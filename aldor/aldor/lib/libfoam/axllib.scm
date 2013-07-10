
;;To read this file in Scheme (load "axllib.boot")
(require 'logical)

(defmacro:load "foam_s.scm")
(defmacro:load "runtime.lsp")

;; (let ((c |g-runtime|)) (write (list '|g-runtime|)) (let ((fun (car c)) (env (cdr c))) (fun env)))

(define (call c . args)
  (let ((fun (car c)) (env (cdr c)))
    ;; use append as we may not be allowed to destroy rest-lists
    (apply fun (append args (list env)))))

(define (axllib:dump dump-to-file)
  (let ((args (dump dump-to-file)))
    (if args
	(begin
	  (display "[AXLLIB available]")
	  (newline)
	  (((mcm) 'toplevel) args))
	(quit))))

