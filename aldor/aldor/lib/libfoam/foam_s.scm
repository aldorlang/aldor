;; as foam_l.lsp but in scheme

;; (in-package "FOAM-USER" :use '("FOAM" "LISP"))

;; TagBody

;; Portable, and nasty, tagbody macro
;; Originally written by Keith Playford, re-hacked by Peter Broadbery

;;  Go for serious continuation abuse here in order to limit stack
;;  use in a compiler that can't deal with tail in general.
;;
(provide 'tagbody)

(define *tagbody-dispatcher-name* 'prog:dispatch-name)

;; actually: before end, or unconditional go
(define (tagbody-until-label forms)
  (cond ((null? forms) '())
	((symbol? (car forms)) (tagbody-until-label (cdr forms)))
	((and (pair? (car forms)) (eq? (car (car forms)) 'go))
	 (list (car forms)))
	(else (cons (car forms) (tagbody-until-label (cdr forms))))))

(define (tagbody-forms-before-a-label forms)
  (cond ((null? forms) '())
	((symbol? (car forms)) '())
	(else (cons (car forms) (tagbody-forms-before-a-label (cdr forms))))))


(define (tagbody-label-forms forms)
    (cond ((null? forms) '())
          ((symbol? (car forms))
           (cons
            (cons (car forms) (cons '() (tagbody-until-label (cdr forms))))
            (tagbody-label-forms (cdr forms))))
          (else (tagbody-label-forms (cdr forms)))))

(define (tagbody-first-label forms)
    (cond ((null? forms) '())
          ((symbol? (car forms)) (car forms))
          (else (tagbody-first-label (cdr forms)))))

(defmacro tagbody forms
    (let ((tag-label-forms (tagbody-label-forms forms)))
      (if (null? tag-label-forms) ; No labels at all
        `(let/cc return ,@forms)
        `(let/cc return
           (let* ((,*tagbody-dispatcher-name* '())
		 (go (lambda (f) (,*tagbody-dispatcher-name* f))))
             (labels
               ,tag-label-forms
               (let/cc dropped-out
                 (labels
                   ((dispatcher (fn)
                      (dispatcher
                        (let/cc called
                          (set! ,*tagbody-dispatcher-name* called)
                          (dropped-out (fn))))))
                   ; these must see the tags
                   (dispatcher
                     (let/cc panic
                       (set! ,*tagbody-dispatcher-name* panic)
                       ,@(tagbody-forms-before-a-label forms)
                       ,(tagbody-first-label forms)))))))))))

(defmacro let/cc (var . forms)
  `(call-with-current-continuation (lambda (,var) ,@forms)))

(defmacro labels (fns . forms)
  `(letrec ,(map (lambda (fn)
		   (list (car fn)
			 (cons 'lambda (cdr fn))))
		 fns)
     ,@forms))


;; other junk...
(defmacro when (condition . forms)
   `(if ,condition ,@forms))


;; We generate nil --- this is probably a bug.

(define nil '())

(defmacro in-package toto nil)

;; Silly debugging junk

(define debug #f)
(define args nil)
(define stack nil)

;; Different words for lisp and scheme
(defmacro eq (a b) `(eq? ,a ,b))

(defmacro setq (var value) `(set! ,var ,value))

(defmacro the (a b) 
  (if (eq a '|Bool|)
      (if (eq b 'nil) #f #t)
      b))
(defmacro code-char (n) `(integer->char ,n))
(defmacro schar (s i) `(string-ref ,s ,i))
(defmacro char (s i) `(string-ref ,s ,i))
(defmacro setf (a b) `(set! ,a ,b))
(defmacro null (x) `(null? ,x))
(defmacro mod (a b) `(modulo ,a ,b))

(define (maybe-cdr x)
  (if (pair? x) (cdr x) x))

;; for formats and that
(require "/usr/axiomxl/gnu/rs/lib/scm/format")


(defmacro cases (var . cases)
  `(case ,var ,@(map (lambda (the-case) `(( ,(car the-case)) ,@(cdr the-case)))
		    cases)))

;; (file-imports --> 
;; (file-exports -->
;; these are functions to relate with axiom, do nothing in scheme

(defmacro declare-prog (name-result params)
   (write (car name-result)) 
   (newline))

(defmacro declare-type (name type)
   (begin 
      (write "The type of ")
      (write name)
      (write " should be ")
      (write type)
      (write ".")
      (newline)
      `(define ,name nil)))

(defmacro defspecials lst
   `(begin ,@(map (lambda (name) `(define ,name nil)) lst)))

(defmacro ignore-var (var) nil)
(defmacro block-return (obj value) `(return ,value))

;; for values

(defmacro values x `(list ,@x))

(defmacro multiple-value-setq (list1 fuca)
   `(let ((*tmp-values* (list->vector ,fuca)))
       ,@(mult-setq list1 0)))

;; fuca is the list returned by values
;; list1 is a list of symbols

;; NB. Vscheme provides multiple values, although 
;; the syntax is odd (see divide, below).

(define (mult-setq lst n)
   (if (null? lst) nil
     (cons `(setq ,(car lst) (vector-ref *tmp-values* ,n))
	   (mult-setq (cdr lst) (+ n 1)))))
       

;; For defprog
;; Closure hashcodes
(define foam-function-list '())

(define alloc-progn-info 
   (lambda (fun val)
      (set! foam-function-list 
            (cons (cons fun val) foam-function-list))))

(define (foam-function-info fun)
   (let ((xx (assoc fun foam-function-list)))
        (if (not xx) 
	    (alloc-progn-info fun (vector nil 0))
            (cdr xx))))

(define (|ProgHashCode| x)
   (let ((aa (foam-function-info x)))
        (if (null? aa) 0
            (vector-ref aa 1))))

(define (|SetProgHashCode| x y)
   (let ((aa (foam-function-info x)))
        (if (null? aa) 0
            (vector-set! aa 1 y))))

;; only to abreviate
(define call/cc call-with-current-continuation)

(defmacro defprog (type temps . body)
   `(begin
       (define ,(caar type)
          (lambda ,(map car (cadr type))
                        (let ((bbb stack)
			      (zzz 
			        (typed-let ,temps
				  (set! args (list ,@(map car (cadr type)))) 
				  (setq stack (cons (list ',(caar type) args) stack))
			          (call/cc (lambda (return) ,@body)))))
			   (set! stack (maybe-cdr bbb))
			   zzz)))
       ;; This is here because (begin (define a 1) 1) defines 'a' in an inner
       ;; scope!
       (define stupid-stupide-scheme 
	 (alloc-progn-info ,(caar type) (vector nil 0)))))

(defmacro typed-let (letvars . forms)
   `(let ,(map (lambda (var) 
		 `( ,(car var) ',(type2init (cadr var))))
               letvars)
         ,@forms))

(define type2init 
   (lambda (x)
      (cond 
         ((eq? x '|Char|) '|CharInit|)
         ((eq? x '|Bool|) '|BoolInit|)
         ((eq? x '|Byte|) '|ByteInit|)
         ((eq? x '|HInt|) '|HIntInit|)
         ((eq? x '|SInt|) '|SIntInit|)
         ((eq? x '|BInt|) '|BIntInit|)
         ((eq? x '|SFlo|) '|SFloInit|)
         ((eq? x '|DFlo|) '|DFloInit|)
         ((eq? x '|Arr|) '|ArrInit|)
         ((eq? x '|Record|) '|RecordInit|)
         ((eq? x '|Env|) '|EnvInit|)
;;         ((eq? x '|Clos|) '|ClosInit|)
         ((eq? x '|Ptr|) '|PtrInit|)
         ((eq? x '|Word|) '|WordInit|)
         ((eq? x '|Arb|) '|ArbInit|)
         ((eq? x '|Level|) '|LevelInit|)
         (else (write (list 'unknown x))
	       nil))))

;; file-exports and file-imports are only for axiom
;; nothing to do in scheme

(defmacro file-imports (l) nil)
(defmacro file-exports (l) nil)

;; Well.. We have a garbage collector
(defmacro |FoamFree| (o)  #f)

;; For CCall, the same as in lsp
(defmacro |CCall| (fun . args)
   `(let ((c ,fun))
         (let ((fun (|FunProg| (|ClosFun| c)))
               (env (|ClosEnv| c)))
              (funcall fun ,@args env))))

(defmacro funcall (fun . args) `(,fun ,@args))

(defmacro |FunProg| (x) x)
(defmacro |Clos| (x y) `(cons ,y ,x))
(defmacro |ClosFun| (x) `(car ,x))
(defmacro |ClosEnv| (x) `(cdr ,x))
(defmacro |SetClosFun| (x y) `(set-car! ,x ,y))
(defmacro |SetClosEnv| (x y) `(set-cdr! ,x ,y))

(defmacro |MakeEnv| (x y) `(cons ,y (cons ,x nil)))
(defmacro |EnvLevel| (x) `(car ,x))
(defmacro |EnvNext| (x) `(cadr ,x))
(defmacro |EnvInfo| (x) `(cddr ,x))
(defmacro |SetEnvInfo| (x val) `(set-cdr! (cdr ,x) ,val))

(define null-char-string (string (integer->char 0)))

(defmacro |MakeLit| (x) 
    `(string-append ,x null-char-string))
(define  (lit->string l)
    (define (aux n)
       (if (equal? (string-ref l n) (integer->char 0)) nil
           (cons (string-ref l n) (aux (+ n 1)))))
    (list->string (aux 0)))

(defmacro |puts| (x) `(display ,x))

(defmacro |DDecl| (name . args)
  (put! name 'struct-args args)
  #f)
 
(defmacro |RNew| (name)
   (let* ((struct-args (get name 'struct-args))
          (init-args (map (lambda (x) (type2init (cadr x)))
                          struct-args)))
         `(list->vector ',init-args)))

(defmacro |RElt| (name field index rec)
	  `(vector-ref ,rec ,index))

(defmacro |SetRElt| (name field index rec val)
   `(vector-set! ,rec ,index ,val))

(defmacro |ANew| (type size)
   (if (eq type '|Char|)
	`(make-string ,size)
   `(make-vector ,size ,(type2init type))))

(defmacro |AElt| (name index)
	  `(thing-ref ,name ,index))

(defmacro |SetAElt| (name index val)
   `(thing-set! ,name ,index ,val))

(define (thing-ref thing index)
   (if (string? thing) (string-ref thing index)
     (vector-ref thing index)))

(define (thing-set! thing index val)
   (if (string? thing) (string-set! thing index val)
      (vector-set! thing index val)))
   	 
(defmacro |MakeLevel| (builder struct)
	`(|RNew| ,struct))

(defmacro |EElt| (accessor n var)
	  `(vector-ref ,var ,n))

 (defmacro |SetEElt| (accessor n var val)
    `(vector-set! ,var ,n ,val))

(defmacro |Lex| (accessor n var)
	  `(vector-ref ,var ,n))

 (defmacro |SetLex| (accessor n var val)
        `(vector-set! ,var ,n ,val))


(defmacro |stdinFile| () '(current-input-port))
(defmacro |stdoutFile| () '(current-output-port))
(defmacro |stderrFile| () '(current-output-port))

(define (|strLength| s)
   (let ((len (length s)))
        (do ((i 0 (+ 1 i))
             (c (string-ref s i) (string-ref s i)))
            ((= i len) nil)
            (if (char=? c |CharCode0|) (return i)))))
           
(define (|formatSInt| n) (|MakeLit| (format #f "~d" n)))
(define (|formatBInt| n) (|MakeLit| (format #f "~d" n)))
(define (|formatSFloat| x) (|MakeLit| (format #f "~f" x)))
(define (|formatDFloat| x) (|MakeLit| (format #f "~f" x)))

;; Printing functions
(define (|printNewLine| cs) (newline cs))
(define (|printChar|  cs c) (write-char c cs))

;needs to stop when it gets a null character
(define (|printString| cs s)
   (let ((len (length s)) (i 0))
        (do ((i 0 (+ 1 i))
            (c (string-ref s i) (string-ref s i)))
            ((= i len) nil)
            (if (char=? c |CharCode0|) (return i)
                (write-char cs c)))))

(define (|printSInt| cs n) (format cs "~d" n))
(define (|printBInt| cs n) (format cs "~d" n))
(define (|printSFloat| cs x) (format cs "~f" x))
(define (|printDFloat| cs x) (format cs "~f" x))

(define (|fputc| si cs)
  (|printChar| cs (code-char si))
  si)

(define (|fputs| s cs)
  (|printString| cs s))

;; read a string into s starting at pos i1, ending at i2
;; we should probably macro-out cases where args are constant

;; fill s[i1..i2] with a null terminated string read from
;; the given input stream
(define (|fgetss| s i1 i2 f)
   (letrec ((aux (lambda (n)
		   (if (= n i2)
		       (begin (string-set! s n (code-char 0))
			      (- n i1))
		       (let ((c (read-char f))) 
			 (string-set! s n c)
			 (if (equal? c #\newline)
			     (begin (string-set! s (+ n 1)
						 (code-char 0))
				    (- n i1))
			     (aux (+ n 1))))))))
     (aux i1)))

;; write s[i1..i2] to the output stream f
;; stop on any null characters

(define (|fputss| s i1 i2 f)
  (letrec ((aux (lambda (n)
		  (if (= n (+ i2 1)) (- n i1)
		      (let ((c (schar s n)))
			(if  (equal? c (code-char 0))
			     (- n i1)
			     (begin (write-char c f)
				    (aux (+ n 1))))))))
	   (aux2 (lambda (n)
		   (let ((c (schar s n)))
		     (if (equal? c (code-char 0))
			 (- n i1)
			 (begin (write-char c f)
				(aux2 (+ n 1))))))))
    (if (< i2 0) (aux2 i1) (aux i1))))



;; type defs for Foam types
(define |Char|  'string-char)
(define |Bool|  '(member t nil))
(define |Byte|  'unsigned-byte)
(define |HInt|  '(#.(- (expt 2 15)) #.(1- (expt 2 15))))
(define |SInt|  'fixnum)
(define |BInt|  'integer)
(define |SFlo|  'short-float)
(define |DFlo|  'long-float)
(define |Arr|   #t)
(define |Record| #t)
(define |Env|   #t)  ; (or cons nil)
;;(define |Clos|  'list)
(define |Ptr|   #t)
(define |Word|  #t)
(define |Arb|   #t)
(define |Level| #t) ;; structure??

;; 16 default values for types.  Used as initializers in lets.
(define |CharInit|    '#\Space)
(define |BoolInit|    #f)
(define |ByteInit|    0)
(define |HIntInit|    0)
(define |SIntInit|    0)
(define |BIntInit|    0)
(define |SFloInit|    0.0s0)
(define |DFloInit|    0.0d0)
(define |ArrInit|     nil)
(define |RecordInit|  nil)
(define |EnvInit|     nil)
;;(define |ClosInit|  nil)
(define |PtrInit|     nil)
(define |WordInit|    nil)
(define |ArbInit|     nil)
(define |LevelInit|   nil)

;; Bool values are assumed to be either 'T or NIL.
;; Thus non-nil values are canonically represented.
(defmacro |BoolFalse|     () #f)
(defmacro |BoolTrue|      () 'T)
(defmacro |BoolNot|      (x) `(NOT ,x))
(defmacro |BoolAnd|    (x y)
  `(let ((xx ,x) (yy ,y)) (AND xx yy))) ;; force evaluation of both args
(defmacro |BoolOr|     (x y)
  `(let ((xx ,x) (yy ,y)) (OR  xx yy))) ;; force evaluation of both args
(defmacro |BoolEQ|     (x y) `(EQ ,x ,y))
(defmacro |BoolNE|     (x y) `(NOT (|BoolEQ| ,x ,y)))

(define |CharCode0| (integer->char 0))

(defmacro |CharSpace|    () '#\Space)
(defmacro |CharNewline|  () '#\Newline)
(defmacro |CharMin|      ()  |CharCode0|)
(defmacro |CharMax| () (integer->char (- char-code-limit 1)))
(defmacro |CharIsDigit| (x) `(char-numeric? ,x))
(defmacro |CharIsLetter| (x) `(char-alphabetic? ,x))
(defmacro |CharLT|    (x y) `(CHAR-CI<?  (the |Char| ,x) (the |Char| ,y)))
(defmacro |CharLE|    (x y) `(CHAR-CI<=? (the |Char| ,x) (the |Char| ,y)))
(defmacro |CharEQ|    (x y) `(CHAR-CI=?  (the |Char| ,x) (the |Char| ,y)))
(defmacro |CharNE|    (x y) `(not (|CharEQ| ,x ,y)))
(defmacro |CharLower|   (x) `(the |Char| (CHAR-DOWNCASE (the |Char| ,x))))
(defmacro |CharUpper|   (x) `(the |Char| (CHAR-UPCASE (the |Char| ,x))))
(defmacro |CharOrd|     (x) `(char->integer ,x))
(defmacro |CharNum|     (x) `(integer->char ,x))

(define most-positive-long-float  3.402823S38)
(define most-negative-long-float -3.402823S38)
(define most-positive-short-float  3.402823S38)
(define most-negative-short-float -3.402823S38)

(define most-negative-fixnum (- (expt 2 30)))
(define most-positive-fixnum    (expt 2 30))

(define long-float-epsilon 6.258487S-8)
(define short-float-epsilon 6.258487S-8)

(defmacro zerop (x) `(zero? ,x))
(defmacro minusp (x) `(negative? ,x))
(defmacro plusp (x) `(positive? ,x))

(defmacro |SFlo0|        () 0.0s0)
(defmacro |SFlo1|        () 1.0s0)
(defmacro |SFloMin|      () most-negative-short-float)
(defmacro |SFloMax|      () most-positive-short-float)
(defmacro |SFloEpsilon|  () short-float-epsilon)
(defmacro |SFloIsZero|  (x) `(zerop (the |SFlo| ,x)))
(defmacro |SFloIsNeg|   (x) `(minusp (the |SFlo| ,x)))
(defmacro |SFloIsPos|   (x) `(plusp (the |SFlo| ,x)))
(defmacro |SFloLT|    (x y) `(< (the |SFlo| ,x) (the |SFlo| ,y)))
(defmacro |SFloLE|    (x y) `(<= (the |SFlo| ,x) (the |SFlo| ,y)))
(defmacro |SFloEQ|    (x y) `(= (the |SFlo| ,x) (the |SFlo| ,y)))
(defmacro |SFloNE|    (x y) `(not (|SFloEQ| ,x ,y)))
(defmacro |SFloNegate|  (x) `(the |SFlo| (- (the |SFlo| ,x))))
(defmacro |SFloNext|    (x) `(the |SFlo| (+ (the |SFlo| ,x) 1.0s0)))
(defmacro |SFloPrev|    (x) `(the |SFlo| (- (the |SFlo| ,x) 1.0s0)))
(defmacro |SFloMinus| (x y) `(the |SFlo| (- (the |SFlo| ,x) (the |SFlo| ,y))))
(defmacro |SFloTimes| (x y) `(the |SFlo| (* (the |SFlo| ,x) (the |SFlo| ,y))))
(defmacro |SFloTimesPlus| (x y z)
   `(the |SFlo| (+ (* (the |SFlo| ,x) (the |SFlo| ,y)) (the |SFlo| ,z))))
(defmacro |SFloDivide|  (x y) `(the |SFlo| (/ (the |SFlo| ,x) (the |SFlo| ,y))))
(defmacro |SFloRPlus|  (x y r) '(error "unimplemented operation -- SFloRPlus"))
(defmacro |SFloRMinus| (x y r) '(error "unimplemented operation -- SFloRTimes"))
(defmacro |SFloRTimes| (x y r) '(error "unimplemented operation -- SFloRTimes"))
(defmacro |SFloRTimesPlus| (x y z r) '(error "unimplemented operation -- SFloTimesPlus"))
(defmacro |SFloRDivide|(x y r) '(error "unimplemented operation -- SFloDivide"))
(defmacro |SFloDissemble| (x) '(error "unimplemented operation -- SFloDissemble"))
(defmacro |SFloAssemble| (w x y) '(error "unimplemented operation -- SFloAssemble"))

(defmacro |DFlo0|         () 0.0d0)
(defmacro |DFlo1|         () 1.0d0)
(defmacro |DFloMin|       () most-negative-long-float)
(defmacro |DFloMax|       () most-positive-long-float)
(defmacro |DFloEpsilon|   () long-float-epsilon)
(defmacro |DFloIsZero|   (x) `(zerop (the |DFlo| ,x)))
(defmacro |DFloIsNeg|    (x) `(minusp (the |DFlo| ,x)))
(defmacro |DFloIsPos|    (x) `(plusp (the |DFlo| ,x)))
(defmacro |DFloLE|     (x y) `(<= (the |DFlo| ,x) (the |DFlo| ,y)))
(defmacro |DFloEQ|     (x y) `(= (the |DFlo| ,x) (the |DFlo| ,y)))
(defmacro |DFloLT|     (x y) `(< (the |DFlo| ,x) (the |DFlo| ,y)))
(defmacro |DFloNE|     (x y) `(not (|DFloEQ| ,x ,y)))
(defmacro |DFloNegate|   (x) `(the |DFlo| (- (the |DFlo| ,x))))
(defmacro |DFloNext|     (x) `(the |DFlo| (+ (the |DFlo| ,x) 1.0d0)))
(defmacro |DFloPrev|     (x) `(the |DFlo| (- (the |DFlo| ,x) 1.0d0)))
(defmacro |DFloPlus|   (x y) `(the |DFlo| (+ (the |DFlo| ,x) (the |DFlo| ,y))))
(defmacro |DFloMinus|  (x y) `(the |DFlo| (- (the |DFlo| ,x) (the |DFlo| ,y))))
(defmacro |DFloTimes|  (x y) `(the |DFlo| (* (the |DFlo| ,x) (the |DFlo| ,y))))
(defmacro |DFloDivide| (x y) `(the |DFlo| (/ (the |DFlo| ,x) (the |DFlo| ,y))))
(defmacro |DFloTimesPlus| (x y z)
  `(the |DFlo| (+ (* (the |DFlo| ,x) (the |DFlo| ,y)) (the |DFlo| ,z))))
(defmacro |DFloRPlus|  (x y r) '(error "unimplemented operation -- DFloRPlus"))
(defmacro |DFloRMinus| (x y r) '(error "unimplemented operation -- DFloRTimes"))
(defmacro |DFloRTimes| (x y r) '(error "unimplemented operation -- DFloRTimes"))
(defmacro |DFloRTimesPlus| (x y z r) '(error "unimplemented operation -- DFloTimesPlus"))
(defmacro |DFloRDivide|(x y r) '(error "unimplemented operation -- DFloDivide"))
(defmacro |DFloDissemble| (x)  '(error "unimplemented operation -- DFloDissemble"))
(defmacro |DFloAssemble| (w x y z) '(error "unimplemented operation -- DFloAssemble"))

(defmacro |Byte0|         () 0)
(defmacro |Byte1|         () 1)
(defmacro |ByteMin|       () 0)
(defmacro |ByteMax|       () 255)

(defmacro |HInt0|         () 0)
(defmacro |HInt1|         () 1)
(defmacro |HIntMin|       () #.(- (expt 2 15)))
(defmacro |HIntMax|       () #.(1- (expt 2 15)))

(defmacro evenp (x) `(even? ,x))
(defmacro oddp (x) `(odd? ,x))
(defmacro mod (n1 n2) `(modulo ,n1 ,n2))
(defmacro rem (n1 n2) `(remainder ,n1 ,n2))
(defmacro declare toto #f)
(defmacro type toto #f)

(defmacro |SInt0|         () 0)
(defmacro |SInt1|         () 1)
(defmacro |SIntMin|       () `(the |SInt| most-negative-fixnum))
(defmacro |SIntMax|       () `(the |SInt| most-positive-fixnum))
(defmacro |SIntIsZero|   (x) `(zerop (the |SInt| ,x)))
(defmacro |SIntIsNeg|    (x) `(minusp (the |SInt| ,x)))
(defmacro |SIntIsPos|    (x) `(plusp (the |SInt| ,x)))
(defmacro |SIntIsEven|   (x) `(evenp (the |SInt| ,x)))
(defmacro |SIntIsOdd|    (x) `(oddp (the |SInt| ,x)))
(defmacro |SIntLE|     (x y) `(<= (the |SInt| ,x) (the |SInt| ,y)))
(defmacro |SIntEQ|     (x y) `(= (the |SInt| ,x) (the |SInt| ,y)))
(defmacro |SIntLT|     (x y) `(< (the |SInt| ,x) (the |SInt| ,y)))
(defmacro |SIntNE|     (x y) `(not (|SIntEQ| ,x ,y)))
(defmacro |SIntNegate|   (x) `(the |SInt| (- (the |SInt| ,x))))
(defmacro |SIntPrev|      (x) `(the |SInt| (1- (the |SInt| ,x))))
(defmacro |SIntNext|      (x) `(the |SInt| (1+ (the |SInt| ,x))))
(defmacro |SIntPlus|   (x y) `(the |SInt| (+ (the |SInt| ,x) (the |SInt| ,y))))
(defmacro |SIntMinus|  (x y) `(the |SInt| (- (the |SInt| ,x) (the |SInt| ,y))))
(defmacro |SIntTimes|  (x y) `(the |SInt| (* (the |SInt| ,x) (the |SInt| ,y))))
(defmacro |SIntTimesPlus| (x y z)
  `(the |SInt| (+ (* (the |SInt| ,x) (the |SInt| ,y)) (the |SInt| ,z))))
(defmacro |SIntMod|    (x y) `(the |SInt| (mod (the |SInt| ,x)(the |SInt| ,y))))
(defmacro |SIntQuo|    (x y)
  `(the |SInt| (quotient (the |SInt| ,x) (the |SInt| ,y))))
(defmacro |SIntRem|    (x y) 
  `(the |SInt| (remainder (the |SInt| ,x) (the |SInt| ,y))))
;;! declare all let variables
(defmacro |SIntDivide| (x y) `(call-with-values (lambda () (divide ,x ,y))
                                  (lambda (a b) (values a b))))
(defmacro |SIntGcd|  (x y) `(the |SInt| (gcd (the |SInt| ,x) (the |SInt| ,y))))

 (defmacro |SIntPlusMod|  (a b c)
  `(the |SInt| (mod (+ (the |SInt| ,a) (the |SInt| ,b)) (the |SInt| ,c))))
(defmacro |SIntMinusMod| (a b c)
  `(the |SInt| (mod (- (the |SInt| ,a) (the |SInt| ,b)) (the |SInt| ,c))))
(defmacro |SIntTimesMod| (a b c)
  `(the |SInt| (mod (* (the |SInt| ,a) (the |SInt| ,b)) (the |SInt| ,c))))
(defmacro |SIntLength|  (x) `(the |SInt| (integer-length (the |SInt| ,x))))
(defmacro |SIntShiftUp| (x y) `(the |SInt| (ash (the |SInt| ,x) (the |SInt| ,y))))
(defmacro |SIntShiftDn| (x y) `(the |SInt| (ash (the |SInt| ,x) (the |SInt| (- (the |SInt| ,y))))))

;; OJO
;; (defmacro |SIntBit|   (x i)
;;  `(let ((xx ,x) (ii ,i)) (declare (type |SInt| xx ii)) (logbitp ii xx)))

(defmacro |SIntNot|     (a) `(the |SInt| (lognot (the |SInt| ,a))))
(defmacro |SIntAnd|   (a b)
  `(the |SInt| (my-logand (the |SInt| ,a) (the |SInt| ,b))))
(defmacro |SIntOr|    (a b)
  `(the |SInt| (my-logior (the |SInt| ,a) (the |SInt| ,b))))

(define (my-logand a b) (logand a b))
(define (my-logior a b) (logior a b))

(define (|SIntToBInt| x) x)
(define (|BIntToSInt| x) x)
(define (|ArrToSInt| a) (string->number (lit->string a)))
(define (|ArrToBInt| a) (string->number (lit->string a)))
(define (|ArrToSFlo| a) (string->number (lit->string a)))
(define (|ArrToDFlo| a) (string->number (lit->string a)))

(define (|SIntToDFlo| x)  (exact->inexact x))

(define (|SIntToSFlo| x)  (exact->inexact x))

(define (|BIntToDFlo| x)  (exact->inexact x))

(define (|BIntToSFlo| x)  (exact->inexact x))


(defmacro |BInt0|        () 0)
(defmacro |BInt1|        () 1)
(defmacro |BIntIsZero|  (x) `(zerop (the |BInt| ,x)))
(defmacro |BIntIsNeg|   (x) `(minusp(the |BInt| ,x)))
(defmacro |BIntIsPos|   (x) `(plusp (the |BInt| ,x)))
(defmacro |BIntIsEven|  (x) `(evenp (the |BInt| ,x)))
(defmacro |BIntIsOdd|   (x) `(oddp  (the |BInt| ,x)))
;;(defmacro |BIntIsSingle| (x) `(typep ,x '|SInt|))
(defmacro |BIntLE|    (x y) `(<= (the |BInt| ,x) (the |BInt| ,y)))
(defmacro |BIntEQ|    (x y) `(=  (the |BInt| ,x) (the |BInt| ,y)))
(defmacro |BIntLT|    (x y) `(<  (the |BInt| ,x) (the |BInt| ,y)))
(defmacro |BIntNE|    (x y) `(not (|BIntEQ| ,x ,y)))
(defmacro |BIntNegate|  (x) `(the |BInt| (-   (the |BInt| ,x))))
(defmacro |BIntPrev|     (x) `(the |BInt| (1-  (the |BInt| ,x))))
(defmacro |BIntNext|     (x) `(the |BInt| (1+  (the |BInt| ,x))))
(defmacro |BIntPlus|  (x y) `(the |BInt| (+ (the |BInt| ,x) (the |BInt| ,y))))
(defmacro |BIntMinus| (x y) `(the |BInt| (- (the |BInt| ,x) (the |BInt| ,y))))
(defmacro |BIntTimes| (x y) `(the |BInt| (* (the |BInt| ,x) (the |BInt| ,y))))
(defmacro |BIntTimesPlus| (x y z)
  `(the |BInt| (+ (* (the |BInt| ,x) (the |BInt| ,y)) (the |BInt| ,z))))
(defmacro |BIntMod|   (x y)
	 `(the |BInt| (mod (the |BInt| ,x)(the |BInt| ,y))))
(defmacro |BIntQuo|   (x y)
  `(the |BInt| (quotient (the |BInt| ,x) (the |BInt| ,y))))

(defmacro |BIntRem|   (x y)
  `(the |BInt| (remainder (the |BInt| ,x) (the |BInt| ,y))))
(defmacro |BIntDivide| (x y)
	`(call-with-values (lambda () (divide ,x ,y))
                           (lambda (a b) (values a b))))

(defmacro |BIntGcd|   (x y)
  `(the |BInt| (gcd (the |BInt| ,x) (the |BInt| ,y))))
(defmacro |BIntSIPower| (x y)
  `(let ((xx ,x) (yy ,y))
     (declare (type |BInt| xx))
     (declare (type |SInt| yy))
     (if (minusp yy) (error "cannot raise integers to negative powers")
	 (the |BInt| (expt xx yy)))))
(defmacro |BIntBIPower| (x y)
  `(let ((xx ,x) (yy ,y))
     (declare (type |BInt| xx))
     (declare (type |BInt| yy))
     (if (minusp yy) (error "cannot raise integers to negative powers")
       (the |BInt| (expt xx yy)))))
(defmacro |BIntLength|  (x) `(the |SInt| (integer-length (the |BInt| ,x))))
(defmacro |BIntShiftUp| (x y) `(the |BInt| (ash (the |BInt| ,x)(the |SInt| ,y))))
(defmacro |BIntShiftDn| (x y) 
   `(the |BInt| (ash (the |BInt| ,x) (- (the |SInt| ,y)))))

;; OJO logbitp
;;(defmacro |BIntBit|   (x i)
;;  `(let ((xx ,x) (ii ,i)) (declare (type |BInt| xx) (type |SInt| ii))
;;        (logbitp ii xx)))

(defmacro |PtrNil|      () '())
(defmacro |PtrIsNil|   (x) `(NULL? ,x))
(defmacro |PtrEQ|    (x y) `(eq ,x ,y))
(defmacro |PtrEQ|    (x y) `(eq ,x ,y))
(defmacro |PtrNE|    (x y) `(not (eq ,x ,y)))

