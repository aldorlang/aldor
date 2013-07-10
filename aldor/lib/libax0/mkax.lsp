--)set bre fast
--)lisp (break)
)fin
(progn

(in-package 'boot)

(setq stubs '(
  (|Export| (|Declare| |DoubleFloat| |SetCategory|) () ())
  (|Export| (|Declare| |Float| |SetCategory|) () ())
  (|Export| (|Declare| |Integer| |IntegralDomain|) () ())
  (|Export| (|Declare| |Symbol| |SetCategory|) () ())
  (|Export| (|Declare| |Fraction|
	     (|Apply| -> (|Declare| |T| |IntegralDomain|) |Field|)) () ())
  (|Export| (|Declare| |Factored|
	     (|Apply| -> (|Declare| |T| |IntegralDomain|) (|With| () ())))
   () ())
  (|Export| (|Declare| |Matrix|
	     (|Apply| -> (|Declare| |T| |Ring|) (|With| () ()))) () ())
  (|Export| (|Declare| |SparseUnivariatePolynomial|
	     (|Apply| -> (|Declare| |T| |Ring|) |IntegralDomain|)) () ())
  (|Export| (|Declare| |Polynomial|
	     (|Apply| -> (|Declare| |T| |Ring|) |IntegralDomain|)) () ())
))

;; A list of all exteded domains in libax0
;; (stubs.as + stubs given above)
(|setExtendedDomains| 
 '(|Boolean| |DoubleFloat| |Equation| |Factored|
   |Float| |Fraction| |InputForm| |Integer| |List| |Matrix|
   |NonNegativeInteger| |Polynomial| |PositiveInteger| |SegmentBinding|
   |SingleInteger| |SparseUnivariatePolynomial| |String| |Symbol|
   |UniversalSegment| |Vector| ))

; Machine generated cliques are represented by a list of symbols.
; Hand-edited files are represented by a list of a list of a symbol.
; These are in reverse dependency order 
(setq cliques '(
;;  ((|patmatch2|))	; Hand-edited
  (|patmatch2|)
  (|fraction|)
  (|fr|)
;;  ((|equation|))	; Hand-edited
  (|equation2|)	
  (|poly|)
  (|fspace|)
  (|multpoly|)
  (|polycat|)
;;  ((|fulleval|))	; Hand-edited
  (|matrix|)
  (|matcat|)
  (|vector|)
  (|integer|)
  (|symbol|)
  (|float|)
  (|si|)
  (|sf|)
;;  ((|patmatch1|))	; Hand-edited
  (|patmatch1|)
  (|variable|)
  (|kl|)
  (|pattern|)
  (|op|)
  (|misc|)
  (|indexedp|)
  (|mkfunc|)
  (|any|)
  (|sex|)
  (|array2|)
  (|array1|)
  (|list|)
  (|seg|)
  (|stream|)
  (|aggcat| |boolean| |string|)
  (|equation1|)	
  (BASE |card| |catdef| |outform| |retract| |trigcat| |void| |coerce|)
))

(defun doit ()
  (let ((tags nil))
    (dolist (clique cliques)
	    ;; (assert (listp clique))
	    ;; (assert (or (symbolp (car clique)) (listp (car clique))))
	    (setq generated (symbolp (car clique)))
	    ;; Compute the tag.
	    (setq tag (if generated (car clique) (caar clique)))
	    (when (eq (car clique) 'BASE)
		  (setq tag '|basecliq|)
		  (setq clique (cdr clique)))
	    (push tag tags)
	    (print clique)
	    (print tag)
	    (print generated)
	    ;; Generate the ax file.
	    (setq |$baseForms| (if (member '|catdef| clique) stubs nil))
	    (if generated
		(|sourceFilesToAxFile| (format nil "ax/~a.ax" tag) clique)))
    ;; Print the list of tags.
    (with-open-file (out "ax.lst" :direction :output)
		    (dolist (tag tags)
			    (format out "~a~%" tag)))))

(doit)
)
