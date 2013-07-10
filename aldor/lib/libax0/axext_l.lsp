
;; File containing primitives needed by exextend in order to interop with axiom
;; This file could do with some declares

(in-package "FOAM-USER" :use '("FOAM" "LISP"))

;; tacky but means we can run programs

(defun H-integer (l e)
  (|AXL-LiteralToInteger| l))
	
(defun  H-string (l e)
  (|AXL-LiteralToString| l))

(defun  H-error (l e)
  (|AXL-error| l))

(eval-when (load eval)
	   (defconstant |G-axclique_string_305639517| (cons #'H-String nil))
	   (defconstant |G-axclique_integer_685864888| (cons #'H-integer nil))
	   (defconstant |G-axclique_error_011667951| (cons #'H-error nil)))

;; Literals should be null-terminated strings

;; SingleInteger

(defmacro |AXL-LiteralToSingleInteger| (l)
  `(parse-integer ,l :junk-allowed t))

(defmacro |AXL-LiteralToInteger| (l)
  `(parse-integer ,l :junk-allowed t))

(defmacro |AXL-LiteralToDoubleFloat| (l)
  `(read-from-string ,l nil (|DFlo0|)
		     :preserve-whitespace t))

(defmacro |AXL-LiteralToString| (l)
  `(subseq ,l 0 (- (length ,l) 1)))

(defmacro |AXL-SingleIntegerToInteger| (si)
  `(coerce (the |SInt| ,si) |BInt|))

(defmacro |AXL-StringToFloat| (s)
  `(boot::|string2Float| ,s))

(defmacro |AXL-IntegerIsNonNegative| (i)
  `(not (< ,i 0)))

(defmacro |AXL-IntegerIsPositive| (i)
  `(< 0 (the |BInt| ,i)))

(defmacro |AXL-plusInteger| (a b)
  `(the |BInt| (+ (the |BInt| ,a)
		  (the |BInt| ,b))))

(defmacro |AXL-minusInteger| (a b)
  `(the |BInt| (- (the |BInt| ,a)
		  (the |BInt| ,b))))

(defmacro |AXL-timesInteger| (a b)
  `(the |BInt| (* (the |BInt| ,a)
		  (the |BInt| ,b))))

(defmacro |AXL-eqInteger| (a b)
  `(= (the |BInt| ,a)
      (the |BInt| ,b)))

(defmacro |AXL-ltInteger| (a b)
  `(< (the |BInt| ,a)
      (the |BInt| ,b)))

(defmacro |AXL-leInteger| (a b)
  `(<= (the |BInt| ,a)
       (the |BInt| ,b)))

(defmacro |AXL-gtInteger| (a b)
  `(> (the |BInt| ,a)
      (the |BInt| ,b)))

(defmacro |AXL-geInteger| (a b)
  `(>= (the |BInt| ,a)
       (the |BInt| ,b)))

(defmacro |AXL-plusSingleInteger| (a b)
  `(the |SInt| (+ (the |SInt| ,a)
		  (the |SInt| ,b))))

(defmacro |AXL-minusSingleInteger| (a b)
  `(the |SInt| (- (the |SInt| ,a)
		  (the |SInt| ,b))))

(defmacro |AXL-timesSingleInteger| (a b)
  `(the |SInt| (* (the |SInt| ,a)
		  (the |SInt| ,b))))

(defmacro |AXL-eqSingleInteger| (a b)
  `(= (the |SInt| ,a)
      (the |SInt| ,b)))

(defmacro |AXL-ltSingleInteger| (a b)
  `(< (the |SInt| ,a)
      (the |SInt| ,b)))

(defmacro |AXL-leSingleInteger| (a b)
  `(<= (the |SInt| ,a)
       (the |SInt| ,b)))

(defmacro |AXL-gtSingleInteger| (a b)
  `(> (the |SInt| ,a)
      (the |SInt| ,b)))

(defmacro |AXL-geSingleInteger| (a b)
  `(>= (the |SInt| ,a)
       (the |SInt| ,b)))

(defmacro |AXL-incSingleInteger| (i)
  `(the |SInt| (+ (the |SInt| ,i) 1)))

(defmacro |AXL-decSingleInteger| (i)
  `(- (the |SInt| ,i)
     (the |SInt| 1)))

(defmacro |AXL-onefnSingleInteger|  () '(the |SInt| 1))
(defmacro |AXL-zerofnSingleInteger| () '(the |SInt| 0))

(defmacro |AXL-cons| (x y)
  `(cons ,x ,y))

(defmacro |AXL-nilfn| () nil)

(defmacro |AXL-car| (x) `(car ,x))

(defmacro |AXL-cdr| (x) `(cdr ,x))

(defmacro |AXL-null?| (x) `(null ,x))

(defmacro |AXL-rplaca| (x y) `(rplaca ,x ,y))

(defmacro |AXL-rplacd| (x y) `(rplacd ,x ,y))

(defmacro |AXL-error| (msg) `(error ,msg))
  
;; arrays
;; 0 based!
(defmacro |AXL-arrayRef| (arr i)
  `(|AElt| ,arr ,i))

(defmacro |AXL-arraySet| (arr i v)
  `(setf (|AElt| ,arr ,i) ,v))

(defmacro |AXL-arrayToList| (x)
  `(coerce ,x 'list))

(defmacro |AXL-arraySize| (x)
  `(length ,x))

(defmacro |AXL-arrayNew| (n)
  `(make-array ,n))

(defmacro |AXL-arrayCopy| (x)
  `(copy-seq ,x))

;; Vectors


;; Testing

(defun |AXL-spitSInt| (x)
  (print x))

