**PROCEDURE: main
**BEGIN: Three Address Code Statements
    a_ = 5
    b_ = 1
Label2:
    temp0 = a_ + 1
    a_ = temp0
    temp1 = b_ < 2
    temp3 = !  temp1
    if(temp3) goto Label1
    temp2 = b_ + 1
    b_ = temp2
    goto Label0
Label1:
    temp4 = b_ - 1
    b_ = temp4
Label0:
    temp5 = a_ < 10
    if(temp5) goto Label2
    write a_
**END: Three Address Code Statements
