#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#define MAXSIZE 1000

typedef union
{
    float fData;
    char cData;
} Item;

typedef struct
{
    int top;
    Item items[MAXSIZE];
} Stack;

Stack* initialize();
void push(Stack*,Item);
Item pop(Stack*);
Item top(Stack*);
int isEmpty(Stack*);
int isFull(Stack*);
void infixToPostfix(char*, char*);
float evaluatePostfix(char*);
int checkStack(Stack*);
int prec(char);
int isLowerPrec(char,char);
void replaceNewLineBySpace(char*);
void error();

int main()
{
    char infixExpr[256]="";
    char postfixExpr[256]="";
    printf("Enter an expression you want to evaluate or Ctrl+Z to exit: ");
    while(fgets(infixExpr,255,stdin)!=NULL)
    {
        replaceNewLineBySpace(infixExpr);
        infixToPostfix(infixExpr,postfixExpr);
        printf("Postfix: %s\n",postfixExpr);
        float result=evaluatePostfix(postfixExpr);
        printf("Result: %f\n\n",result);
        printf("Enter an expression you want to evaluate or Ctrl+Z to exit: ");
    }
    return 0;
}

Stack* initialize ()
{
    Stack*s=malloc(sizeof(Stack));
    s->top=0;
    return s;
}

void push (Stack*s,Item value)
{
    s->items[s->top++]=value;
}

Item pop (Stack*s)
{
    return s->items[--s->top];
}

Item top(Stack*s)
{
    return s->items[s->top-1];
}

int isEmpty (Stack*s)
{
    if(s->top==0)
        return 1;
    else
        return 0;
}

int isFull (Stack*s)
{
    if(s->top==MAXSIZE)
        return 1;
    else
        return 0;
}

void infixToPostfix(char* infix, char* postfix)  //Stack will contain operators
{
    Stack*s=initialize();
    int i,index=0;
    Item temp;
    for(i=0; i<strlen(infix); i++)
    {
        if(infix[i]==' ')
            continue;
        if(isdigit(infix[i]))
        {
            postfix[index++]=infix[i];  //Any number put it directly in the postfix
            if(infix[i+1]==' '|| infix[i+1]=='+' || infix[i+1]=='-' || infix[i+1]=='*' || infix[i+1]=='/' || infix[i+1]=='^' || infix[i+1]==')' || infix[i+1]=='\0')   //To add space after the last digit or check for multiple digits
                postfix[index++]=' ';
        }
        else if(infix[i]=='-' && infix[i+1]!=' ' && !isdigit(infix[i-1]))  //To handle negative numbers
            postfix[index++]=infix[i];
        else if(infix[i]=='.')
            postfix[index++]=infix[i];
        else if(infix[i]=='(')
        {
            temp.cData=infix[i];
            push(s,temp);
        }
        else if(infix[i]==')')
        {
            while(!isEmpty(s) && top(s).cData!='(')  //pop from stack until open bracket '('
            {
                postfix[index++]=pop(s).cData;
                postfix[index++]=' ';  //To add space after the operator
            }
            if(isEmpty(s) || top(s).cData!='(')
                error();
            pop(s);  //Get rid of open bracket '('
        }
        else  //operator
        {
            while(!isEmpty(s) && top(s).cData!='(' && isLowerPrec(infix[i],top(s).cData))
            {
                temp.cData=pop(s).cData;
                if(temp.cData!='(')   //*****
                {
                postfix[index++]=temp.cData;
                postfix[index++]=' ';
                }
            }
            temp.cData=infix[i];
            push(s,temp);
        }
    }
    while(!isEmpty(s))  //remaining
    {
        if(top(s).cData=='(')
            error();
        postfix[index++]=pop(s).cData;
        postfix[index++]=' ';
    }
    postfix[index]=0;  // NULL character to indicate the end of the string (postfix expression)
}


float evaluatePostfix(char*postfix)  //Stack will contain numbers
{
    Stack*s=initialize();
    int i,flag=0;
    Item op1,op2,res,temp;
    for(i=0; i<strlen(postfix); i++)
    {
        if(postfix[i]==' ')
            continue;
        else if(isdigit(postfix[i]) || (postfix[i]=='-' && postfix[i+1]!=' '))
        {
            temp.fData=0;
            int power=-1,checkPoint=1;
            if(postfix[i]=='-')
            {
                flag=1;
                continue;
            }
            while(isdigit(postfix[i]) || postfix[i]=='.')
            {
                if(postfix[i]=='.')
                {
                    i++;
                    continue;
                }
                if(postfix[i-checkPoint]=='.')
                {
                    temp.fData+=(postfix[i]-'0')*pow(10.0,power);  //To convert a string of a single or multiple digits to a numerical value
                    power--;
                    checkPoint++;  // i shifts then checkPoint must also shift
                }
                else
                    temp.fData=temp.fData*10.0+(postfix[i]-'0');  //for single and multiple digits
                i++;
            }
            i--;
            if(flag==0)
                push(s,temp);
            else
            {
                temp.fData*=-1.0;
                push(s,temp);
                flag=0;
            }
        }
        else  //operator
        {
            if(!isEmpty(s))
                op1=pop(s);
            else
            {
                error();
                break;
            }
            if(!isEmpty(s))
                op2=pop(s);
            else
            {
                error();
                break;
            }
            switch(postfix[i])
            {
            case '+':
                res.fData=op1.fData+op2.fData;
                push(s,res);
                break;
            case '-':
                res.fData=op2.fData-op1.fData;
                push(s,res);
                break;
            case '*':
                res.fData=op1.fData*op2.fData;
                push(s,res);
                break;
            case '/':
                res.fData=op2.fData/op1.fData;
                push(s,res);
                break;
            case '^':
                res.fData=pow(op2.fData,op1.fData);
                push(s,res);
                break;
            default:
                error();
                break;
            }

        }
    }
    int count=checkStack(s);
    if(count>1)
    {
        error();  //Because the stack should contain only one element at the end
        exit(-1);
    }
    else
        return pop(s).fData;
}

int checkStack(Stack*s)  //To check whether the stack contains more than one element or not
{
    int count=0;
    Stack*temp=initialize();
    while(!isEmpty(s))
    {
        push(temp,pop(s));
        count++;
    }
    while(!isEmpty(temp))
        push(s,pop(temp));
    return count;
}

int prec(char op)
{
    switch(op)
    {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    case '^':
        return 3;
    default:
        error();
        return -1;
    }
}

int isLowerPrec(char op1,char op2)
{
    return prec(op1)<=prec(op2);
}

void replaceNewLineBySpace(char*s)
{
    char*s1=s;
    while((s1=strstr(s1,"\n"))!=NULL)
        *s1=' ';
}

void error()
{
    printf("Error in Expression\n");
    exit(-1);
}