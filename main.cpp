#include <string>
#include <vector>

#include <iostream>
#include <cmath>

#include <chrono>



template <typename T> void Print(T data, std::string end="\n")
{
    if (end=="\n")
    {
        std::cout<<data<<std::endl;
    }
    else
    {
        std::cout<<data<<end;
    }
}

class Timer
{

private:

    std::chrono::high_resolution_clock::time_point dt = std::chrono::high_resolution_clock::now();

public:

    double delta;



    double Tick()
    {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(now - dt);
        dt = now;
        delta = elapsed_time.count();
        return delta;
    }
};

class Parser
{

private:

    enum STATES { PAR, EXP, MUL, DIV, ADD, SUB };
    std::vector<std::string> symbols = { "()","^","*","/","+","-","P" };
    std::string tab_spacing="  ";
    int state = 1;
    bool show_work = true;
    bool show_expression = false;

public:

    std::string GetTabSpacing()
    {
        return tab_spacing;
    }

    bool IsNumber(std::string data)
    {
        int dc = 0;
        for (int i=0; i<data.size(); i++)
        {
            char c = data.at(i);
            if (c=='.')
            {
                dc++;
            }
            if ((c<'0' || c>'9') && c!='.')
            {
                return false;
            }
        }
        if (dc>1)
        {
            return false;
        }
        return true;
    }

    bool IsNegativeNumber(std::string data)
    {
        int dc = 0;
        if (data[0]!='-') { return false; }
        for (int i=0; i<data.size(); i++)
        {
            char c = data.at(i);
            if (c=='.')
            {
                dc++;
            }
            if ((c<'0' || c>'9') && c!='.')
            {
                return false;
            }
        }
        if (dc>1)
        {
            return false;
        }
        return true;
    }

    bool IsValidNumber(std::string data)
    {
        return (IsNumber(data) || IsNegativeNumber(data));
    }

    bool IsOperation(std::string data)
    {
        if (data.size()==1)
        {
            char c = data.at(0);
            if ((c>41 && c<46) || c=='^' || c=='/')
            {
                return true;
            }
        }
        return false;
    }

    bool IsValidCharacter(char c)
    {
        if ((c>39 && c<58) || c=='^' || c=='/' || c=='.')
        {
            return true;
        }
        return false;
    }

    void ToggleShowWork()
    {
        show_work=!show_work;
        show_expression=!show_work;
    }

    std::vector<std::string> Error(std::string text, std::string type)
    {
        return {type+" Error: "+text};
    }

    void PrintTokens(std::vector<std::string> tokens)
    {
        Print(symbols[state], tab_spacing);
        for (int i=0; i<tokens.size(); i++)
        {
            Print(tokens[i], " ");
        }
        Print("");
    }

    std::string RandomExpression()
    {
        bool generating = true, can_finish = false;
        int number_count = 0, limit=5;
        std::string expression = "";
        while (generating)
        {
            std::string token = "";
            if (IsNumber(token)) { number_count++; }
            if (number_count>=limit) { can_finish = true; }
            expression += token;
        }
        return expression;
    }

    std::string GetNumberString(std::string A)
    {
        int decimal=A.size();
        for (int i=0; i<A.size(); i++) { if (A[i]=='.') { decimal=i; break; } }
        while(A[A.size()-1]=='0' && A.size()>decimal)
        {
            A=A.substr(0, A.size()-1);
        }
        if (A[A.size()-1]=='.')
        {
            A=A.substr(0, A.size()-1);
        }
        return A;
    }

    void PrintProblem(std::string op, std::string A, std::string B, std::string C)
    {
        A=GetNumberString(A);
        B=GetNumberString(B);
        C=GetNumberString(C);
        const int maximum = std::max(std::max(std::max(A.size(), B.size()+1), C.size()), (const long unsigned int) 8);
        std::string a_ = "";
        std::string b_ = "";
        std::string c_ = "";
        std::string separator = "";
        for (int i=A.size(); i<maximum; i++) { a_+=" "; }
        for (int i=B.size()+1; i<maximum; i++) { b_+=" "; }
        for (int i=C.size(); i<maximum; i++) { c_+=" "; }
        for (int i=0; i<maximum; i++) { separator+="_"; }
        Print(a_+A);
        Print(op+b_+B);
        Print(separator);
        Print(c_+C);
    }



    std::string SolveUnary(std::string op, std::string A)
    {
        std::string work = op+tab_spacing+op+" "+GetNumberString(A);
        if (op=="-")
        {
            if (show_work)
            {
                try
                {
                    Print(work+" = "+GetNumberString(std::to_string(std::stod(A)*-1.0))); return std::to_string(std::stod(A)*-1.0);
                }
                catch (std::invalid_argument) { return "Error"; }
            }
        }
        return A;
    }

    std::string SolveProblem(std::string op, std::string A, std::string B)
    {
        if (A.size()>2) { if (A[0]=='-' && A[1]=='-') { A=A.substr(2, A.size()-3); } }
        if (B.size()>2) { if (B[0]=='-' && B[1]=='-') { B=B.substr(2, B.size()-3); } }
        double a, b;
        try
        {
            a=std::stod(A);
            b=std::stod(B);
        }
        catch (std::invalid_argument)
        {
            return SolveUnary(op, A);
            //return "Error";
        }
        float c=0.0;
        if      (op=="^") { c=pow(a,b);}
        else if (op=="*") { c=    a*b; }
        else if (op=="/") { c=    a/b; }
        else if (op=="+") { c=    a+b; }
        else if (op=="-") { c=    a-b; }
        if (show_work) { PrintProblem(op, A, B, std::to_string(c)); }
        return std::to_string(c);
    }

    std::vector<std::string> SolveExpression(std::vector<std::string> tokens)
    {
        if (tokens.size()==2)
        {
            tokens = {SolveUnary(tokens[0], tokens[1])};
        }
        else if (tokens.size()%2==0)
        {
            if (tokens.size()>1)
            {
                if (tokens[0]=="-" && IsValidNumber(tokens[1]))
                {
                    tokens.erase(tokens.begin());
                    tokens[0]=std::to_string(std::stod(tokens[0])*-1.0);
                }
            }
            //return tokens;
        }
        int operation_index = -1;
        std::string op = symbols[state];
        for (int i=0; i<tokens.size(); i++)
        {
            if (IsOperation(tokens[i]))
            {
                if      (state==MUL) { if (tokens[i]=="*" || tokens[i]=="/") { operation_index = i; break; } }
                else if (state==DIV) { if (tokens[i]=="*" || tokens[i]=="/") { operation_index = i; break; } }
                else if (state==ADD) { if (tokens[i]=="+" || tokens[i]=="-") { operation_index = i; break; } }
                else if (state==SUB) { if (tokens[i]=="+" || tokens[i]=="-") { operation_index = i; break; } }
                else                 { if (tokens[i]==op) { operation_index = i; break; } }
            }
        }
        if (operation_index>-1)
        {
            int cull = operation_index-1;
            std::string result;
            try
            {
                std::string A = tokens[operation_index-1];
                std::string B = tokens[operation_index+1];
                result = SolveProblem(tokens[operation_index], A, B);
            }
            catch (std::length_error) { result="Length"; }
            if (result=="Error")
            {
                return Error("Mangled Expression", "Problem");
            }
            else if (result=="Length")
            {
                return Error("Mangled Expression", "Expression");
            }
            else
            {
                for (int i=0; i<3; i++)
                {
                    tokens.erase(tokens.begin()+cull);
                }
                tokens.insert(tokens.begin()+cull, result);
            }
        }
        else
        {
            state++;
        }
        return tokens;
    }



    std::vector<std::string> Tokenize(std::string data)
    {
        std::vector<std::string> tokens;
        std::string glyph = "", token = "";
        bool can_push = false;
        for (int i=0; i<data.size(); i++)
        {
            char c = data.at(i);
            if (IsValidCharacter(c))
            {
                if      (c==' ') { can_push = true; }
                else if (c=='(') { can_push = true; glyph = "("; }
                else if (c==')') { can_push = true; glyph = ")"; }
                else if (c=='^') { can_push = true; glyph = "^"; }
                else if (c=='*') { can_push = true; glyph = "*"; }
                else if (c=='/') { can_push = true; glyph = "/"; }
                else if (c=='+') { can_push = true; glyph = "+"; }
                else if (c=='-') { can_push = true; glyph = "-"; }
                else
                {
                    if ((c>='0' && c<='9') || c=='.')
                    {
                        token+=c;
                    }
                }
                if (can_push)
                {
                    if (token.size()>0)
                    {
                        if (token.size()==1 && token.at(0)!=' ')
                        {
                            tokens.push_back(token); token="";
                        }
                        else
                        {
                            tokens.push_back(token);
                            token="";
                        }
                    }
                    can_push = false;
                    if (glyph!="")
                    {
                        tokens.push_back(glyph); glyph="";
                    }
                }
            }
        }
        if (token.size()>0)
        {
            if (token.size()==1 && token.at(0)!=' ')
            {
                tokens.push_back(token); token="";
            }
            else
            {
                tokens.push_back(token); token="";
            }
        }
        tokens = ParseNegatives(tokens);
        return tokens;
    }

    std::vector<std::string> ParseNegatives(std::vector<std::string> tokens)
    {
        bool can_continue = false; std::string last = "";
        for (int i=0; i<tokens.size(); i++) { if (tokens[i]=="-" || tokens[i]=="-(") { if (last=="-" || last=="-(") { can_continue = true; } last = tokens[i]; } }
        if (!can_continue) { return tokens; }
        std::string token = "";
        while (true)
        {
            bool found_negative = false;
            for (int i=0; i<tokens.size(); i++)
            {
                std::string token=tokens[i];
                if (i>0 && i<tokens.size())
                {
                    std::string _t=tokens[i-1];
                    std::string t_=tokens[i+1];
                    if (token=="-")
                    {
                        if (_t=="(" || _t=="^" || _t=="*" || _t=="/" || _t=="+" || _t=="-" || _t==".")
                        {
                            found_negative = true;
                            std::string n = tokens[i]+tokens[i+1];
                            tokens.erase(tokens.begin()+i);
                            tokens[i] = n;
                        }
                    }
                }
                else
                {
                    if (token=="-")
                    {
                        found_negative = true;
                        std::string n = tokens[i]+tokens[i+1];
                        tokens.erase(tokens.begin()+i);
                        tokens[i] = n;
                    }
                }
            }
            if (!found_negative) { break; }
        }
        while (true)
        {
            int cull = -1;
            for (int i=0; i<tokens.size(); i++)
            {
                if (tokens[i]=="-(")
                {
                    cull=i;
                }
            }
            if (cull>-1)
            {
                tokens[cull] = "-";
                tokens.insert(tokens.begin()+cull+1, "(");
            }
            else
            {
                break;
            }
        }
        return tokens;
    }

    std::vector<std::string> ParseErrors(std::vector<std::string> tokens)
    {
        int L=0, R=0;
        for (int i=0; i<tokens.size(); i++)
        {
            if      (tokens[i]=="(") { L++; }
            else if (tokens[i]==")") { R++; }
        }
        std::string _L = " [Left="+std::to_string(L);
        std::string _R = " Right="+std::to_string(R)+"]";
        if (L!=R)
        {
            std::string error="Mismatched Parentheses"+_L+_R;
            return Error(error, "Parentheses");
        }
        return tokens;
    }

    std::vector<std::string> ParseParentheses(std::vector<std::string> tokens)
    {
        tokens = ParseErrors(tokens);
        if (tokens.size()==0)
        {
            return Error("No Input", "User");
        }
        int _count = 2;
        while(_count>0)
        {
            state=6;
            if (show_expression)
            {
                PrintTokens(tokens);
            }
            state=1;
            bool found_end = false;
            int li=-1, ri=-1, lc=0, rc=0;
            for (int i=0; i<tokens.size(); i++)
            {
                if      (tokens[i]=="(")
                {
                    lc++;
                    if (i>li && !found_end)
                    {
                        li=i;
                    }
                }
                else if (tokens[i]==")")
                {
                    if (!found_end)
                    {
                        ri=i;
                    }
                    found_end=true;
                }
            }
            if (li<0 || ri<0)
            {
                break;
            }
            std::vector<std::string> expression;
            int diff = ri-li;
            for (int i=0; i<diff+1; i++)
            {
                std::string t = tokens[li];
                if (t!="(" && t!=")")
                {
                    expression.push_back(t);
                }
                tokens.erase(tokens.begin()+li);
            }
            bool has_neg = false;
            for (int i=0; i<expression.size(); i++) { if (expression[i]=="-") { has_neg = true; } }
            if (has_neg) { expression = ParseNegatives(expression); }
            while (expression.size()>1)
            {
                expression=SolveExpression(expression);
                //if (tokens.size()==1) { return tokens; }
            }
            if (expression.size()>0)
            {
                tokens.insert(tokens.begin()+li, expression[0]);
            }
            _count=lc+rc;
            if (tokens.size()==1) { return tokens; }
        }
        return tokens;
    }

    std::string Parse(std::string data)
    {
        std::vector<std::string> tokens = Tokenize(data);
        std::vector<std::string> problem = ParseParentheses(tokens);
        if (problem.size()==1)
        {
            return problem[0];
        }
        else
        {
            state = 1;
            tokens = problem;
        }
        std::string result = "";
        int last_size = tokens.size();
        while(tokens.size()>1)
        {
            if (show_work && tokens.size()!=last_size)
            {
                PrintTokens(tokens);
            }
            tokens = SolveExpression(tokens);
            last_size = tokens.size();
        }
        result = tokens[0];
        return result;
    }

};



int main(int argc, char *argv[])
{
    Parser parser = Parser();
    Timer timer = Timer();
    std::string expression = "";
    if (argc>1)
    {
        for (int i=1; i<argc; i++)
        {
            expression += argv[i];
        }
        timer.Tick();
        Print("\n="+parser.GetTabSpacing()+parser.GetNumberString(parser.Parse(expression)));
        Print("\nTook:", " ");
        Print(timer.Tick());
        Print("");
    }
    else
    {
        bool user_engaged = true; while (user_engaged)
        {
            Print("Enter an expression to solve, 'x' to exit, or 's' to toggle showing work");
            for (int i=0; i<4; i++)
            {
                Print("");
            }
            expression="";
            std::getline(std::cin, expression);
            if (expression=="x")
            {
                user_engaged=false; return 0;
            }
            else if (expression=="s")
            {
                parser.ToggleShowWork();
            }
            else
            {
                timer.Tick();
                Print("\n="+parser.GetTabSpacing()+parser.GetNumberString(parser.Parse(expression)));
                Print("\nTook:", " ");
                Print(timer.Tick());
                Print("");
            }
        }
    }
    return 0;
}