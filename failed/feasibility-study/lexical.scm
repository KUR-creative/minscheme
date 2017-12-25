(define a 12)
(define (f a)
  (define (in-f b)
    (display b)))


(define (fac n) (if (= n 1) 1 (* n (fac (- n 1)))))
(newline)
(display (fac 2) )
(newline)
(display (fac 3) )
(newline)
(display (fac 4) )
(newline)
(display (fac 5) )
(newline)
(newline)

(display (car '(define (fac n)
	    (if (= n 1)
		1
		(* n (fac (- n 1)))))))

(newline)
(display (cadr '(define (fac n)
	    (if (= n 1)
		1
		(* n (fac (- n 1)))))))
(define (func g)
  (g 1 2))

(func 1)

