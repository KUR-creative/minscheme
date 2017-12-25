#ifndef ERRORS_H
#define ERRORS_H

typedef enum ErrorKind {
    NO_ERR = 0,
    UNBOUND_VARIABLE,
    NOT_APPLICABLE,
    INCORRECT_ARGNUM,
    TYPE_MISMATCH,
    ILL_FORMED_SPECIAL_FORM,
} ErrorKind;

extern char* unbound_variable_errmsg; // arr[ENUM]로...
extern char* not_applicable_errmsg;
extern char* incorrect_argnum_errmsg;
extern char* type_mismatch_errmsg;
extern char* ill_formed_special_form_errmsg;

#endif
