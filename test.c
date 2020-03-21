int f(int a)
{
    if(a == 0)
    {
        return a;
    }
    return a + f(a - 1);
}

void main()
{
    int a = 10;
    a =  a -2;
    f(a);
}