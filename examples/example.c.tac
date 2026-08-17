**PROCEDURE: main
**BEGIN: Three Address Code Statements
    temp0 = b_ * c_
    a_ = temp0
    temp1 = p_(2)
    b_ = temp1
    temp2 = a_ + c_
    b_ = temp2
    write a_
**END: Three Address Code Statements
**PROCEDURE: p_
**BEGIN: Three Address Code Statements
    temp0 = e_ <= 0
    temp1 = !  temp0
    if(temp1) goto Label2
    stemp0 = 1
    goto Label1
    goto Label2
Label2:
    temp2 = x_ > 0
    temp5 = !  temp2
    if(temp5) goto Label4
    temp4 = e_ - 1
    temp3 = q_(temp4)
    x_ = temp3
    goto Label3
Label4:
Label5:
    temp6 = a_ > b_
    temp10 = !  temp6
    if(temp10) goto Label6
    temp8 = e_ - 1
    temp7 = p_(temp8)
    a_ = temp7
    temp9 = c_ + 1
    x_ = temp9
    goto Label5
Label6:
Label3:
    write b_
    temp11 = c_ + b_
    a_ = temp11
    stemp0 = 1
    goto Label1
Label1:
    return stemp0
**END: Three Address Code Statements
**PROCEDURE: q_
**BEGIN: Three Address Code Statements
    temp0 = d_ <= 0
    temp1 = !  temp0
    if(temp1) goto Label7
    stemp0 = 0
    goto Label0
    goto Label7
Label7:
    temp3 = d_ - 1
    temp2 = q_(temp3)
    a_ = temp2
    temp4 = a_ + 1
    a_ = temp4
    stemp0 = 1
    goto Label0
Label0:
    return stemp0
**END: Three Address Code Statements
