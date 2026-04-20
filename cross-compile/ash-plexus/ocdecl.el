(defun ocdecl ()
  "Update the old style C declarations from the new style ones.  This assumes
that you set up your declarations as follows:
	#ifdef __STDC__
	[ANSI style function prototypes]
	#else
	[Old style function prototypes]
	#endif
Then if you add or change a function, you can edit the ANSI style prototypes
and then run this function to make the old style prototypes match the new
style ones.  Normally bound to ESC D."
  (interactive)
  (let (ostart oend nstart nend+1 eol)
    (end-of-line)
    (search-backward "#ifdef __STDC__")
    (forward-line 1)
    (setq ostart (point))
    (search-forward "#else")
    (beginning-of-line)
    (setq oend (point))
    (forward-line 1)
    (setq nstart (point))
    (search-forward "#endif")
    (beginning-of-line)
    (setq nend+1 (make-marker))
    (set-marker nend+1 (1+ (point)))
    (goto-char nstart)
    (insert (buffer-substring ostart oend))
    (delete-region (point) (1- nend+1))
    (goto-char nstart)
    (while (< (point) (1- nend+1))
      (end-of-line)
      (setq eol (point))
      (beginning-of-line)
      (re-search-forward "[a-zA-Z0-9_] *(" eol)
      (setq ostart (point))
      (backward-char 1)
      (forward-sexp 1)
      (delete-region ostart (1- (point)))
      (forward-line 1))
    (set-marker nend+1 nil)
    nil))

(define-key esc-map "D" 'ocdecl)
