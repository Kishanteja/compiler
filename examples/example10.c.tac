**PROCEDURE: main
**BEGIN: Three Address Code Statements
    a_ = 5
    b_ = 1
    temp0 = 3 > 2
    temp1 = !  temp0
    if(temp1) goto Label0
    stemp0 = a_
    goto Label1
Label0:
    stemp0 = 4
Label1:
    temp2 = stemp0 > 5
    temp3 = !  temp2
    if(temp3) goto Label3
    c_ = 10
    goto Label2
Label3:
    temp4 = a_ == 5
    temp5 = !  temp4
    if(temp5) goto Label4
    c_ = 2
    goto Label4
Label4:
Label2:
**END: Three Address Code Statements
