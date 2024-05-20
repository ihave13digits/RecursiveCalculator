#include <string>
#include <vector>

#include <iostream>
#include <cmath>



template <typename T> void Print(T data, std::string end="\n") { if (end=="\n") { std::cout<<data<<std::endl; } else { std::cout<<data<<end; } }



class Parser
{

private:

    enum STATES { PAR, EXP, MUL, DIV, ADD, SUB };
    std::vector<std::string> symbols = { "()","^","*","/","+","-","P" };
    int state = 1;
    bool show_work = true;

public:

    bool IsNumber(std::string data)
    { int dc = 0; for (int i=0; i<data.size(); i++) { char c = data.at(i); if (c=='.') { dc++; } if ((c<'0' || c>'9') && c!='.') { return false; } } if (dc>1) { return false; } return true; }
    bool IsOperation(std::string data) { if (data.size()==1) { char c = data.at(0); if ((c>41 && c<46) || c=='^' || c=='/') { return true; } } return false; }
    bool IsValidCharacter(char c) { if ((c>39 && c<58) || c=='^' || c=='/' || c=='.') { return true; } return false; }

    void ToggleShowWork() { show_work=!show_work; }
    std::vector<std::string> Error(std::string text, std::string type) { return {type+" Error: "+text}; }
    void PrintTokens(std::vector<std::string> tokens) { Print(symbols[state], " "); for (int i=0; i<tokens.size(); i++) { Print(tokens[i], ""); } Print(""); }



    std::string SolveProblem(std::string op, std::string A, std::string B)
    {
        if (show_work) { Print(A+" "+op+" "+B); }
        double a=std::stod(A), b=std::stod(B);
        float c=0.0;
        if      (op=="^") { c=pow(a,b);}
        else if (op=="*") { c=    a*b; }
        else if (op=="/") { c=    a/b; }
        else if (op=="+") { c=    a+b; }
        else if (op=="-") { c=    a-b; }
        return std::to_string(c);
    }

    std::vector<std::string> SolveExpression(std::vector<std::string> tokens)
    {
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
            std::string A = tokens[operation_index-1];
            std::string B = tokens[operation_index+1];
            std::string result = SolveProblem(tokens[operation_index], A, B);
            for (int i=0; i<3; i++) { tokens.erase(tokens.begin()+cull); }
            tokens.insert(tokens.begin()+cull, result);
        }
        else { state++; }
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
                else { if (c!=' ') { token+=c; } }
                if (can_push)
                {
                    if (token.size()>0)
                    {
                        if (token.size()==1 && token.at(0)!=' ') { tokens.push_back(token); token=""; }
                        else { tokens.push_back(token); token=""; }
                    }
                    can_push = false;
                    if (glyph!="") { tokens.push_back(glyph); glyph=""; }
                }
            }
        }
        if (token.size()>0)
        {
            if (token.size()==1 && token.at(0)!=' ') { tokens.push_back(token); token=""; }
            else { tokens.push_back(token); token=""; }
        }
        tokens = ParseNegatives(tokens);
        return tokens;
    }

    std::vector<std::string> ParseNegatives(std::vector<std::string> tokens)
    {
        std::string token = "";
        while (true)
        {
            bool found_negative = false;
            std::string _t=""; token="";
            for (int i=0; i<tokens.size(); i++)
            {
                token=tokens[i];
                if (i>0 && i<tokens.size())
                {
                    _t=tokens[i-1];
                    if (token=="-")
                    {
                        if (_t=="(" || _t==")" || _t=="^" || _t=="*" || _t=="/" || _t=="+" || _t=="-" || _t==".")
                        { found_negative = true; std::string n = tokens[i]+tokens[i+1]; tokens.erase(tokens.begin()+i); tokens[i] = n; }
                    }
                }
                else
                {
                    if (token=="-") { found_negative = true; std::string n = tokens[i]+tokens[i+1]; tokens.erase(tokens.begin()+i); tokens[i] = n; }
                }
            }
            if (!found_negative) { break; }
        }
        return tokens;
    }

    std::vector<std::string> ParseErrors(std::vector<std::string> tokens)
    {
        int L=0, R=0, N=0, O=0;
        for (int i=0; i<tokens.size(); i++)
        {
            if      (tokens[i]=="(") { L++; }
            else if (tokens[i]==")") { R++; }
            else if (tokens[i]=="^") { O++; }
            else if (tokens[i]=="*") { O++; }
            else if (tokens[i]=="/") { O++; }
            else if (tokens[i]=="+") { O++; }
            else if (tokens[i]=="-") { O++; }
            else if (IsNumber(tokens[i])) { N++; }
        }
        if (L!=R) { return Error("Mismatched Parentheses", "Parentheses"); }
        if (O>=N) { return Error("Invalid Expression", "Expression"); }
        return tokens;
    }

    std::vector<std::string> ParseParentheses(std::vector<std::string> tokens)
    {
        tokens = ParseErrors(tokens);
        int _count = 2;
        while(_count>0)
        {
            state=6; if (show_work) { PrintTokens(tokens); } state=1;
            bool found_end = false; int li=-1, ri=-1, lc=0, rc=0;
            for (int i=0; i<tokens.size(); i++)
            {
                if      (tokens[i]=="(") { lc++; if (i>li && !found_end) { li=i; } }
                else if (tokens[i]==")") { if (!found_end) { ri=i; } found_end=true; }
            }
            if (li<0 || ri<0) { break; }
            std::vector<std::string> expression; int diff = ri-li;
            for (int i=0; i<diff+1; i++) { std::string t = tokens[li]; if (t!="(" && t!=")") { expression.push_back(t); } tokens.erase(tokens.begin()+li); }
            while (expression.size()>1) { expression=SolveExpression(expression); }
            if (expression.size()>0) { tokens.insert(tokens.begin()+li, expression[0]); }//lc--; rc--; }
            _count=lc+rc;
        }
        return tokens;
    }

    std::string Parse(std::string data)
    {
        std::vector<std::string> tokens = Tokenize(data), problem = ParseParentheses(tokens);
        if (problem.size()==1) { return problem[0]; }
        else { state=1; tokens = problem; }
        std::string result = "";
        while(tokens.size()>1) { if (show_work) { PrintTokens(tokens); } tokens = SolveExpression(tokens); }
        result = tokens[0];
        return result;
    }

};



int main(int argc, char *argv[])
{
    Parser parser = Parser();
    std::string expression = "";
    if (argc>1) { for (int i=1; i<argc; i++) { expression += argv[i]; } Print(parser.Parse(expression)); }
    else
    {
        bool user_engaged = true; while (user_engaged)
        {
            Print("Enter an expression to solve, 'x' to exit, or 's' to toggle showing work");
            for (int i=0; i<4; i++) { Print(""); } expression = ""; std::getline(std::cin, expression);
            if (expression=="x") { user_engaged=false; return 0; } else if (expression=="s") { parser.ToggleShowWork(); } else { Print(parser.Parse(expression)); }
        }
    }
    return 0;
}