**PROCEDURE: main
**BEGIN: Three Address Code Statements
    read a_
    read b_
    temp0 = a_ > b_
    temp2 = !  temp0
    if(temp2) goto Label1
    temp1 = a_ + b_
    n_ = temp1
    goto Label0
Label1:
    temp3 = a_ - b_
    n_ = temp3
Label0:
    write n_
**END: Three Address Code Statements
