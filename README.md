robot
=====

##An autonomous (and remote controlled) robot that weeds your lawn!

General coding practices:
-------------------------
{there is a space between the function type, name, and formal parameter list}

    void function_name (void); //comments have one space between them and the code on the same line.

{There is a space after commas anywhere in the code}

    void function (int example1, char example2);

{Operators have spaces between them and whatever they are operating on}

    ans = number * number2;
    vs
    ans = number*number2

{Especially the * operator, since it can be confused with the dereference operator}

    if (ans == (1 * *num))
    vs
    if(ans == (1**num))

{Braces open on the line beneath a switch/if/for/ect statement}

    switch (something)
    {
    }
    vs
    switch (something) {
    }

