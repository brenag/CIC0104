/*
Aluno: Emanuel Couto Brenag
Matrícula: 190057131
Disciplina: Software Básico
Professor: Bruno Luiggi Macchiavello Espinoza
Editor de Texto: Visual Studio Code
Sistema Operacional: Windows 10
Compilador: MinGW gcc g++ 6.3.0
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <algorithm>
#include <ctype.h> // para usar isdigit
#include <iterator>

using namespace std;

// declaração das structs

struct code_line 
{
    string label;
    string operation;
    string argument1;
    string argument2;
};

struct table_item 
{
    bool is_def = 0;
    vector<int> pendencies_list;
    int address = -1;
};

//DECLARAÇÃO DAS TABELAS 

//Tabela de Instruções 
map<string, int> IT = 
{
    {"ADD",    1},   
    {"SUB",    2},
    {"MUL",    3},
    {"DIV",    4},
    {"JMP",    5},
    {"JMPN",   6},
    {"JMPP",   7},
    {"JMPZ",   8},
    //Apenas o Copy tem 2 operadores
    {"COPY",   9},
    {"LOAD",   10},
    {"STORE",  11},
    {"INPUT",  12},
    {"OUTPUT", 13},
    {"STOP",   14}
};

//Tabela de Diretivas 
// LEMBRAR DE ALTERAR OS ESPAÇOS DE SPACE, POIS PODE SER SPACE 2
map<string, int> DT = 
{
    {"SPACE",   1},
    {"CONST",   1},
};

//Tabela de Simbolos
//Chave do map é uma string e valor é int <string, int>
map<string, table_item> ST;

//Declaração do protótipo das funções

void VerifyOperation(code_line SeparatedLine, int current_line, map<string, table_item>& table);
code_line DivideLine(string str, int current_line);
int OperationSize(string instrucao, string argument);
string uppercase(string str);
void SinglePass(string fileName);
bool VerifyLabel(const string& label, map<string, table_item>& table);
bool VerifyLabel2(const string& label, map<string, int>& table);
void Solve_Pendency(map<string, table_item>& table);
void divide_vector(string argument);
void section_data_first();
void Parser(code_line SeparatedLine, int current_line);
bool isNumber(const string& str);

string more_than_one_label[2];
string vectorization_aux[2];
int program_counter = 0;
int error = 0;
bool label_previous_line = false;
string previous_label = "";
string more_than_2_arguments = "";
vector<int> object_code;
vector<int> object_code_aux;
vector<bool> is_opcode;
vector<bool> is_in_sec_data;
bool is_data_first;
int data_sec_size = 0;
int text_sec_size = 0;

int main(int argc, char **argv)
{
    string fileName = argv[1]; 
    int fileSize = fileName.size();
    if(fileName.substr( fileSize - 4, fileSize) == ".asm")
    {
        SinglePass(fileName); 
        
        cout << "Código Objeto gerado: " <<endl;
        for (auto i = object_code.begin(); i != object_code.end(); ++i)
            cout << *i << " ";
        if(is_data_first == true)
        {
            cout << endl;
            cout << "Codigo Objeto gerado com section data ao final: " <<endl;
            for (auto i = object_code_aux.begin(); i != object_code_aux.end(); ++i)
            {
                cout << *i << " ";
            }
            cout << endl;
            
            
        }
        return 0;
    }
    else
    {
        cout << " O arquivo selecionado nao eh um \".asm\". " << endl;
        return 1;
    }
}

void SinglePass(string fileName)
{   
    int current_line = 1;
    string FileLine;
    //Abertura do arquivo .asm de entrada
    ifstream input_file;
    //Caso queira colocar em pastas diferentes, só colocar input_file.open("Input/" + fileName);
    //Abertura do arquivo de saída .obj
    ofstream output_file;
    input_file.open(fileName);
    output_file.open(fileName.substr( 0, fileName.size() - 4 ) + ".obj");
    //Se o arquivo abrir
    if(input_file.is_open())
    {
        while(!input_file.eof())
        {
            getline(input_file,FileLine);
            //Passa a linha dividida pra struct
            code_line SeparatedLine = DivideLine(uppercase(FileLine),current_line);
            
            //CHAMA O PARSER
            Parser(SeparatedLine, current_line);
            
        
            //SEÇÃO DAS LABELS E PARA VERIFICAÇÃO DE ERROS LexicoS (SCAN) em LABELS

            
            if(!SeparatedLine.label.empty())
            {  
                //Erro Semantico de rotulo repetido
                //if (VerifyLabel(SeparatedLine.label, ST)) não é mais isso, agora usa bool
                if (ST[SeparatedLine.label].is_def)
                {
                    cout << "[Erro Semantico na linha " << current_line << "] - Declaracao rotulo repetido (" << SeparatedLine.label << ")." << endl;
                    error++;
                }
                else if (VerifyLabel(SeparatedLine.label, ST))
                {   
                    //Insere novo label VÁLIDO na tabela de Símbolos e seta como definido
                    ST[SeparatedLine.label].address = program_counter;
                    ST[SeparatedLine.label].is_def = 1;
                }
                else 
                {
                    //Insere novo label VÁLIDO na tabela de Símbolos e seta como definido
                    ST[SeparatedLine.label].address = program_counter;
                    ST[SeparatedLine.label].is_def = 1;
                }
            }



            //SEÇÃO DAS OPERAÇÕES E PARA VERIFICAÇÃO DE ERROS SintaticoS DE DIRETIVAS E INSTRUÇÕES INVÁLIDAS
            
            if(!SeparatedLine.operation.empty())
            {   
                
                if(VerifyLabel2(SeparatedLine.operation,IT))
                {
                    VerifyOperation(SeparatedLine, current_line, ST);

                    program_counter += OperationSize(SeparatedLine.operation, SeparatedLine.argument1);
                    
                }
                else if(VerifyLabel2(SeparatedLine.operation,DT))
                {
                    VerifyOperation(SeparatedLine, current_line, ST);
                    program_counter += OperationSize(SeparatedLine.operation, SeparatedLine.argument1);
                    
                }
                
                
                //VER O QUE PRECISA FAZER PARA CADA SEÇÃO
                else if(SeparatedLine.operation == "SECTION")
                {
                    if(SeparatedLine.argument1 == "DATA" && SeparatedLine.argument2.empty())
                    {
                        is_data_first = false;
                    }
                    else if(SeparatedLine.argument1 == "TEXT" && SeparatedLine.argument2.empty())
                    {
                        is_data_first = true;
                    }
                    else
                    {
                        cout << "[Erro Sintatico na linha " << current_line << "] - A diretiva SECTION " << SeparatedLine.argument1 << " fornecida eh invalida." << endl;
                        error++;
                    }
                }
                

                else
                {
                    //Identifica o erro de intrução/diretiva inválida
                    cout << "[Erro Sintatico na linha " << current_line << "] - A instrucao/diretiva " <<SeparatedLine.operation << " fornecida eh invalida." << endl;
                    error++;
                    VerifyOperation(SeparatedLine, current_line, ST);
                    program_counter += OperationSize(SeparatedLine.operation, SeparatedLine.argument1);

                    //TALVEZ NÃO DEVA COLOCAR VERIFYOPERATION AQUI
                    //VERIFICAR NECESSIDADE DE TESTAR ERROS LexicoS EM INSTRUÇÕES INVÁLIDAS
                    //VerifyOperation(SeparatedLine, current_line, program_counter, ST);
                }
            }  
            more_than_2_arguments = "";
            more_than_one_label[0] = "";   
            more_than_one_label[1] = "";
            current_line++;
        } 
    }
    //Se o arquivo não abrir 
    else
    {
        cout << "[Error] Nao foi possível abrir o arquivo." << endl;
        exit(1);
    }

    //Chama função pra completar tabela de pendências 
    Solve_Pendency(ST);

    //PREENCHE código objeto no arquivo .obj
    
    if(is_data_first == false)
    {
        for (auto i = object_code.begin(); i != object_code.end(); ++i)
        {
            if(*i < 10)
            {
                output_file << "0" << *i << " ";
            }
            else
            {
                output_file << *i << " ";
            }
        }
    }  
    else if(is_data_first == true)
    {
        section_data_first();
        for (auto p = object_code_aux.begin(); p != object_code_aux.end(); ++p)
        {
            if(*p < 10)
            {
                output_file << "0" << *p << " ";
            }
            else
            {
                output_file << *p << " ";
            }
        }
    }
    
    cout << "O Arquivo objeto foi gerado com um total de [" << error << "] erros."<< endl;
    input_file.close();
    output_file.close();
}


//DECLARAÇÃO DAS FUNÇÕES AUXILIARES UTILIZADAS NA PASSAGEM


//Converte string em uppercase e resolve o problema do CASE SENSITIVE
string uppercase(string str) 
{
    transform(str.begin(), str.end(),str.begin(), ::toupper);
    return str;
}


//Verifica o tamanho das operações para incrementar o Program Counter
int OperationSize(string instrucao, string argument)
{
    if(instrucao == "COPY")
    {
        return 3;
    }
    else if(instrucao == "STOP" || instrucao == "CONST")
    {
        return 1;
    }
    //VERIFICA O TAMANHO DE SPACE  
    else if(instrucao == "SPACE")
    {
        if(argument.empty())
        {
            return 1;
        }
        else{
            if(isNumber(argument))
            {
                int size_op = stoi(argument);
                return size_op;
            }
            else
            {
                return 1;
            }
        }
    }

    else if(instrucao == "SECTION")
    {
        return 0;
    }
    else
    {
        return 2;
    }
}


//Função que retorna Struct com os termos de uma linha separados
code_line DivideLine(string str, int current_line)
{
    //Variáveis IGNORE são utilizadas para ignorar espaços, tabulações,
    char ignore_space = ' '; 
    char ignore_tab = '\t';
    char ignore_colon = ':';
    char ignore_comma = ',';
    //string more_than_one_label[5];
    int cont_colon = 0;
    

    //buffer para acumular caracteres de uma palavra e depois passar para o array 
    string buffer = "";

    //Array de strings vazias para preencher com rotulos, operadores e operandos a serem extraídos de uma linha
    string separation_aux[10] = {"","","",""};

    int i = 0;
    
    //Declaração da string que contém rotulos, operadores e operandos de uma linha de código
    code_line SeparatedLine;

    //Verifica se na linha existe um rotulo 
    if(string::npos == str.find(':'))
    {
        //Não tendo rotulo, começa pelo índice 1 do separation_aux, pulando o rotulo
        i = 1; 
    }
    
    for(auto eachChar:str)
	{
        //TESTA SE TEM DOIS rotulos NA MESMA LINHA
        //cout << "valor de cont_colon:" << i << endl;
        if(eachChar == ignore_colon)
        {
            cont_colon++;
        }
        //TROCAR BREAK DENTRO DO IF 
        //Verifica se o caractere é um ponto e vírgula. Se for, encerra a linha, pois ignora comentários.
        if(eachChar == ';')
        {
            break;
        }
        //Verifica se não é um caractere a ser ignorado e armazena no buffer
		if(eachChar != ignore_space && eachChar != ignore_tab && eachChar != ignore_colon && eachChar != ignore_comma)
        {
            buffer+=eachChar; 
        }

        //VERIFICAR SE ELE IGNORA QUANDO TEM DOIS ESPAÇOS SEGUIDOS E SE NECESSITA DISSO!

        //Se caractere for espaço, tab ou vírgula, pula salva palavra do buffer e pula para o próximo índice do vetor
        if( (eachChar == ignore_space || eachChar == ignore_tab || eachChar == ignore_comma ||eachChar == ignore_colon)  && buffer != "" )
        { 
            separation_aux[i] = buffer;
            buffer = "";
            i++;
        }
	}
    separation_aux[i] = buffer;

    //VERIFICADOR DE DOIS OU MAIS rotulos EM UMA ÚNICA LINHA
    //Se a contagem de ":" apontar mais de 1, é pelo fato de existir mais de um rotulo
    if(cont_colon >= 2)
    {
        //passa 2º label declarado para vetor auxiliar
        for(int w = 1; w < cont_colon; w++)
        {
            more_than_one_label[w-1] = separation_aux[w];
        }
        //puxa pros índices corretos para corrigir onde estão a operação e os argumentos
        for(int y = 1; y < i; y++)
        {
            separation_aux[y] = separation_aux[y+(cont_colon-1)];
        }
        //TALVEZ NÃO SEJA DE FATO NECESSÁRIO
        for(int z = 0; z < (cont_colon - 1); z++)
        {
            separation_aux[i-z] = "";
        }
        //reduz o valor de i para o número descontando os rotulos extras passados para more_than_one_label
        i = i - cont_colon + 1;
        error++;
        cout << "[Erro Sintatico na linha " << current_line << "] - " << cont_colon << " rotulos na mesma linha." << endl;

        //SE O rotulo EXTRA JÁ ESTIVER NA TABELA DE SÍMBOLOS APONTA ERRO DE DECLARAÇÃO REPETIDA
        if(ST[more_than_one_label[0]].is_def)
        {
            error++;
            cout << "[Erro Semantico na linha " << current_line << "] - Declaracao de rotulo repetido (" << more_than_one_label[0] << ")." << endl;
        }
        //Se não tiver, passa o endereço e valida is_def
        else
        {
            ST[more_than_one_label[0]].address = program_counter;
            ST[more_than_one_label[0]].is_def = 1;
        }
    }

    //Se i<4, quer dizer que o número de operadores é menor que o máximo (que seria 3, no caso de COPY)    
    if(i < 4)
    {
        separation_aux[4] = "";
    }

    //Passa os argumentos do array para os elementos da struct 
    SeparatedLine.label = separation_aux[0];
    SeparatedLine.operation = separation_aux[1];
    SeparatedLine.argument1 = separation_aux[2];
    SeparatedLine.argument2 = separation_aux[3];
    more_than_2_arguments = separation_aux[4];
    
    //Se label_previous_line == true (LABEL preenchido na linha anterior e pula linha):
    if(label_previous_line == true)
    {   
        //Se a linha seguinte não está vazia
        if(!SeparatedLine.label.empty())
        {   
            //Verifica se ambos rotulos pra mesma linha são iguais ou se o rotulo anterior é repetido
            if ((SeparatedLine.label == previous_label) || ST[previous_label].is_def)
            {
                //Verificar se de fato é erro Sintatico ou Semantico
                cout << "[Erro Semantico na linha " << current_line << "] - Declaracao de rotulo repetido (" << previous_label << ")." << endl;
                cout << "[Erro Sintatico na linha " << current_line << "] - 2 rotulos na mesma linha." << endl;
                error++;
                error++;
                label_previous_line = false;
            }
            //SE O rotulo DA LINHA ANTERIOR AINDA NÃO ESTIVER DEFINIDO
            else
            {
                //Se o rotulo da linha já for definido, aponta os erros e passa como label o label da linha de cima
                if(ST[SeparatedLine.label].is_def)
                {
                    cout << "[Erro Semantico na linha " << current_line << "] - Declaracao de rotulo repetido (" << SeparatedLine.label << ")." << endl;
                    cout << "[Erro Sintatico na linha " << current_line << "] - 2 rotulos na mesma linha." << endl;
                    error++;
                    error++;
                    SeparatedLine.label = previous_label;
                    label_previous_line = false;
                }
                //se nenhum dos dois rotulos tiverem definidos, define e passa endereço de ambos pra tabela de símbolos e aponta erro
                else
                {
                    ST[SeparatedLine.label].address = program_counter;
                    ST[SeparatedLine.label].is_def = true;
                    SeparatedLine.label = previous_label;
                    //Verificar se de fato é erro Sintatico ou Semantico
                    cout << "[Erro Sintatico na linha " << current_line << "] - 2 rotulos na mesma linha." << endl;
                    error++;
                    label_previous_line = false;
                }
            }
        }
        else if(!str.empty() && SeparatedLine.label.empty()) 
        {
            SeparatedLine.label = previous_label;
            label_previous_line = false;
        }
    }
    //Se a linha preencheu o label e continuou na linha seguinte, label_previous_line == true
    if(!SeparatedLine.label.empty() && SeparatedLine.operation.empty() && SeparatedLine.argument1.empty() && SeparatedLine.argument2.empty())
    {
        previous_label = SeparatedLine.label;
        label_previous_line = true;
        SeparatedLine.label = "";
    }
    return SeparatedLine;
}


//Função para verificar se uma chave/string pertence a uma das tabelas.
bool VerifyLabel(const string& label,  map<string, table_item>& table)
{
    auto search_label = table.find(label);
    if (search_label != table.end())
        return 1;
    else
        return 0;
}


bool VerifyLabel2(const string& label,  map<string, int>& table)
{
    auto search_label2 = table.find(label);
    if (search_label2 != table.end())
        return 1;
    else
        return 0;
}

//PODERÁ SER UTILIZADA PARA VERIFICAR ERRO Sintatico!!!
//Função que verifica o número de operandos de cada uma das operações
void VerifyOperation(code_line SeparatedLine, int current_line, map<string, table_item>& table)
{
    //INSTRUÇÃO COPY

    if(SeparatedLine.operation == "COPY")
    {   
        is_opcode.push_back(1);
        is_opcode.push_back(0);
        is_opcode.push_back(0);
        object_code.push_back(IT[SeparatedLine.operation]);
        //Verifica se copy tem os dois argumentos
        if(!SeparatedLine.argument1.empty() && !SeparatedLine.argument2.empty())
        {
            //SE TIVER MAIS DE 2 ARGUMENTOS, APONTA ERRO
            if(!more_than_2_arguments.empty())
            {
                cout << "[Erro Sintatico na linha " << current_line << "] - A quantidade de operandos da instrucao COPY esta incorreta." << endl;
                error++;
            }
            //return 1;
        }
        else
        {
            cout << "[Erro Sintatico na linha " << current_line << "] - A quantidade de operandos da instrucao COPY esta incorreta." << endl;
            error++;
            //return 0;
        }
        
        //Manipulações da Tabela de Símbolos
        //Verifica se não é um vetor
        //Para argument1
        if(!SeparatedLine.argument1.empty())
        {
            if(string::npos == SeparatedLine.argument1.find('+'))
            {
                //Se não está na TS, mas indefinido
                if(!VerifyLabel(SeparatedLine.argument1, ST))
                {
                    ST[SeparatedLine.argument1].pendencies_list.push_back(program_counter+1);
                    ST[SeparatedLine.argument1].is_def = 0;

                    //MANIPULA O CÓDIGO OBJETO
                    object_code.push_back(-1);
                }
                //Se já está na TS e definido
                else if (ST[SeparatedLine.argument1].is_def)
                {
                    //MANIPULA O CÓDIGO OBJETO
                    object_code.push_back(ST[SeparatedLine.argument1].address);
                }
                //Se já está na TS, mas indefinido
                else
                {
                    ST[SeparatedLine.argument1].pendencies_list.push_back(program_counter+1);
                    //MANIPULA O CÓDIGO OBJETO 
                    object_code.push_back(-1);  
                }
            }
            //Se for um vetor
            else if(string::npos != SeparatedLine.argument1.find('+'))
            {
                divide_vector(SeparatedLine.argument1);
                if(!VerifyLabel(vectorization_aux[0], ST))
                {
                    ST[vectorization_aux[0]].pendencies_list.push_back(program_counter+1);
                    ST[vectorization_aux[0]].is_def = 0;
                    //MANIPULA O CÓDIGO OBJETO
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1-stoi(vectorization_aux[1])); 
                    }    
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1);
                    }
                }
                //Se já está na TS e definido
                else if (ST[vectorization_aux[0]].is_def)
                {
                    //MANIPULA O CÓDIGO OBJETO
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(ST[vectorization_aux[0]].address + stoi(vectorization_aux[1]));
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(ST[vectorization_aux[0]].address);
                    }
                }
                //Se já está na TS, mas indefinido
                else
                {
                    ST[vectorization_aux[0]].pendencies_list.push_back(program_counter+1);
                    //MANIPULA O CÓDIGO OBJETO 
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1-stoi(vectorization_aux[1]));  
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1);
                    }
                }
            }
        }
        //Para argument2
        if(!SeparatedLine.argument2.empty())
        {
            if(string::npos == SeparatedLine.argument2.find('+'))
            {
                if(!VerifyLabel(SeparatedLine.argument2, ST))
                {
                    ST[SeparatedLine.argument2].pendencies_list.push_back(program_counter+2);
                    ST[SeparatedLine.argument2].is_def = 0;
                    //MANIPULA O CÓDIGO OBJETO   
                    
                    object_code.push_back(-1);
                }
                else if (ST[SeparatedLine.argument2].is_def)
                {
                    //MANIPULA O CÓDIGO OBJETO   
                    object_code.push_back(ST[SeparatedLine.argument2].address);
                }
                else
                {
                    ST[SeparatedLine.argument2].pendencies_list.push_back(program_counter+2);
                    //MANIPULA O CÓDIGO OBJETO  
                    object_code.push_back(-1);  
                }
            }

            else if(string::npos != SeparatedLine.argument2.find('+'))
            {
                divide_vector(SeparatedLine.argument2);
                
                if(!VerifyLabel(vectorization_aux[0], ST))
                {
                    ST[vectorization_aux[0]].pendencies_list.push_back(program_counter+2);
                    ST[vectorization_aux[0]].is_def = 0;
                    //MANIPULA O CÓDIGO OBJETO   
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1-stoi(vectorization_aux[1])); 
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1);
                    }
                }
                else if (ST[vectorization_aux[0]].is_def)
                {
                    //MANIPULA O CÓDIGO OBJETO   
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(ST[vectorization_aux[0]].address + stoi(vectorization_aux[1]));
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(ST[vectorization_aux[0]].address);
                    }
                }
                else
                {
                    ST[vectorization_aux[0]].pendencies_list.push_back(program_counter+2);
                    //MANIPULA O CÓDIGO OBJETO  
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1-stoi(vectorization_aux[1])); 
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1);
                    }
                }
            }
        }
        //Para supostos argument3 (more_that_2_arguments)
        if(!more_than_2_arguments.empty())
        {
            if(string::npos == more_than_2_arguments.find('+'))
            {
                if(!VerifyLabel(more_than_2_arguments, ST))
                {
                    ST[more_than_2_arguments].pendencies_list.push_back(program_counter+2);
                    ST[more_than_2_arguments].is_def = 0;
                    //MANIPULA O CÓDIGO OBJETO   
                    object_code.push_back(-1);
                }
                else if (ST[more_than_2_arguments].is_def)
                {
                    //MANIPULA O CÓDIGO OBJETO   
                    object_code.push_back(ST[more_than_2_arguments].address);
                }
                else
                {
                    ST[more_than_2_arguments].pendencies_list.push_back(program_counter+2);
                    //MANIPULA O CÓDIGO OBJETO  
                    object_code.push_back(-1);  
                }
            }
            else if(string::npos != more_than_2_arguments.find('+'))
            {
                divide_vector(more_than_2_arguments);
                if(!VerifyLabel(vectorization_aux[0], ST))
                {
                    ST[vectorization_aux[0]].pendencies_list.push_back(program_counter+2);
                    ST[vectorization_aux[0]].is_def = 0;
                    //MANIPULA O CÓDIGO OBJETO   
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1-stoi(vectorization_aux[1])); 
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1);
                    }
                }
                else if (ST[vectorization_aux[0]].is_def)
                {
                    //MANIPULA O CÓDIGO OBJETO   
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(ST[vectorization_aux[0]].address + stoi(vectorization_aux[1]));
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(ST[vectorization_aux[0]].address);
                    }
                }
                else
                {
                    ST[vectorization_aux[0]].pendencies_list.push_back(program_counter+2);
                    //MANIPULA O CÓDIGO OBJETO  
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1-stoi(vectorization_aux[1])); 
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1);
                    }
                }
            }
        }
    }
    
    //MODIFICAR STOP PARA O CASO DE RECEBER ARGUMENTO E PASSAR PARÂMETRO PARA TABELA DE SÍMBOLOS
    //INSTRUÇÃO STOP
    else if(SeparatedLine.operation == "STOP")
    {
        is_opcode.push_back(1);
        object_code.push_back(IT[SeparatedLine.operation]);
        if(SeparatedLine.argument1.empty() &&  SeparatedLine.argument2.empty())
        {            
            //TUDO CERTO, SEM PROBLEMAS!
        }
        else if(!SeparatedLine.argument1.empty() && SeparatedLine.argument2.empty() || !SeparatedLine.argument1.empty() && !SeparatedLine.argument2.empty() )
        {   
            cout << "[Erro Sintatico na linha " << current_line << "] - A quantidade de operandos da instrucao " << SeparatedLine.operation << " esta incorreta." << endl;
            error++;
            
            if(string::npos == SeparatedLine.argument1.find('+'))
            {
                //Se não está na TS, mas indefinido
                if(!VerifyLabel(SeparatedLine.argument1, ST))
                {
                    ST[SeparatedLine.argument1].pendencies_list.push_back(program_counter+1);
                    ST[SeparatedLine.argument1].is_def = 0;

                    //MANIPULA O CÓDIGO OBJETO
                    object_code.push_back(-1);
                }
                //Se já está na TS e definido
                else if (ST[SeparatedLine.argument1].is_def)
                {
                    //MANIPULA O CÓDIGO OBJETO
                    object_code.push_back(ST[SeparatedLine.argument1].address);
                }
                //Se já está na TS, mas indefinido
                else
                {
                    ST[SeparatedLine.argument1].pendencies_list.push_back(program_counter+1);
                    //MANIPULA O CÓDIGO OBJETO 
                    object_code.push_back(-1);  
                }
            }
            //Se for um vetor
            else if(string::npos != SeparatedLine.argument1.find('+'))
            {
                divide_vector(SeparatedLine.argument1);
                if(!VerifyLabel(vectorization_aux[0], ST))
                {
                    ST[vectorization_aux[0]].pendencies_list.push_back(program_counter+1);
                    ST[vectorization_aux[0]].is_def = 0;
                    //MANIPULA O CÓDIGO OBJETO
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1-stoi(vectorization_aux[1])); 
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1);
                    }
                }
                //Se já está na TS e definido
                else if (ST[vectorization_aux[0]].is_def)
                {
                    //MANIPULA O CÓDIGO OBJETO
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(ST[vectorization_aux[0]].address + stoi(vectorization_aux[1]));
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(ST[vectorization_aux[0]].address);
                    }
                }
                //Se já está na TS, mas indefinido
                else
                {
                    ST[vectorization_aux[0]].pendencies_list.push_back(program_counter+1);
                    //MANIPULA O CÓDIGO OBJETO 
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1-stoi(vectorization_aux[1]));  
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1);
                    }
                }
            }
        }
    }

    //DIRETIVA SPACE
    else if(SeparatedLine.operation == "SPACE" )
    {
        
        
        if(SeparatedLine.argument1.empty() &&  SeparatedLine.argument2.empty())
        {
            data_sec_size++;
            is_opcode.push_back(0);
            object_code.push_back(0);
        }
        else if (!SeparatedLine.argument1.empty() &&  SeparatedLine.argument2.empty())
        {
            if (isNumber(SeparatedLine.argument1))
            {
                for (int p = 0; p < stoi(SeparatedLine.argument1); p++)
                {
                    data_sec_size++;
                    is_opcode.push_back(0);
                    object_code.push_back(0);
                }
            }
            else
            {
                is_opcode.push_back(0);
                object_code.push_back(0);
            }
        }
        else
        {   
            object_code.push_back(0);
            cout << "[Erro Sintatico na linha " << current_line << "] - A quantidade de operandos da diretiva " << SeparatedLine.operation << " esta incorreta." << endl;
            error++;
            //return 0;
        }
    }

    //DIRETIVA CONST 
    else if (SeparatedLine.operation == "CONST")
    {
        is_opcode.push_back(0);
        data_sec_size++;
        if(!SeparatedLine.argument1.empty() &&  SeparatedLine.argument2.empty())
        {   
            if(isNumber(SeparatedLine.argument1))
            {
                if(stoi(SeparatedLine.argument1)<0)
                {
                    cout << "[Erro Lexico na linha " << current_line << "] - Token invalido na diretiva " << SeparatedLine.operation << "." << endl;
                    error++;
                }
                else
                {
                    object_code.push_back(stoi(SeparatedLine.argument1));
                }
            }
            //return 1;
        }
        else
        {   
            cout << "[Erro Sintatico na linha " << current_line << "] - A quantidade de operandos da diretiva " << SeparatedLine.operation << " esta incorreta." << endl;
            error++;
            //return 0;
        }
    }

    //OUTRAS INSTRUÇÕES
    //Verifica se os outros tem apenas 1 argumento
    else
    {
        is_opcode.push_back(1);
        is_opcode.push_back(0);
        object_code.push_back(IT[SeparatedLine.operation]);
        if(!SeparatedLine.argument1.empty() &&  SeparatedLine.argument2.empty())
        {
            //return 1;
        }
        else
        {   
            cout << "[Erro Sintatico na linha " << current_line << "] - A quantidade de operandos da instrucao " << SeparatedLine.operation << " esta incorreta." << endl;
            error++;
            //return 0;
        }

        //Manipulações da Tabela de Símbolos
        //Argument 1
        if(!SeparatedLine.argument1.empty())
        {
            if(string::npos == SeparatedLine.argument1.find('+'))
            {
                //Se não está na TS, mas indefinido
                if(!VerifyLabel(SeparatedLine.argument1, ST))
                {
                    ST[SeparatedLine.argument1].pendencies_list.push_back(program_counter+1);
                    ST[SeparatedLine.argument1].is_def = 0;

                    //MANIPULA O CÓDIGO OBJETO
                    object_code.push_back(-1);
                }
                //Se já está na TS e definido
                else if (ST[SeparatedLine.argument1].is_def)
                {
                    //MANIPULA O CÓDIGO OBJETO
                    object_code.push_back(ST[SeparatedLine.argument1].address);
                }
                //Se já está na TS, mas indefinido
                else
                {
                    ST[SeparatedLine.argument1].pendencies_list.push_back(program_counter+1);
                    //MANIPULA O CÓDIGO OBJETO 
                    object_code.push_back(-1);  
                }
            }
            //Se for um vetor
            else if(string::npos != SeparatedLine.argument1.find('+'))
            {
                divide_vector(SeparatedLine.argument1);
                if(!VerifyLabel(vectorization_aux[0], ST))
                {
                    ST[vectorization_aux[0]].pendencies_list.push_back(program_counter+1);
                    ST[vectorization_aux[0]].is_def = 0;
                    //MANIPULA O CÓDIGO OBJETO
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1-stoi(vectorization_aux[1])); 
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1);
                    }
                }
                //Se já está na TS e definido
                else if (ST[vectorization_aux[0]].is_def)
                {
                    //MANIPULA O CÓDIGO OBJETO
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(ST[vectorization_aux[0]].address + stoi(vectorization_aux[1]));
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(ST[vectorization_aux[0]].address);
                    }
                }
                //Se já está na TS, mas indefinido
                else
                {
                    ST[vectorization_aux[0]].pendencies_list.push_back(program_counter+1);
                    //MANIPULA O CÓDIGO OBJETO 
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1-stoi(vectorization_aux[1]));  
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1);
                    }
                }
            }
        }

        //Argument 2
        //CASO ESTEJA INCORRETO E TENHA ARGUMENTO 2
        if(!SeparatedLine.argument2.empty())
        {
            if(string::npos == SeparatedLine.argument2.find('+'))
            {
                if(!VerifyLabel(SeparatedLine.argument2, ST))
                {
                    ST[SeparatedLine.argument2].pendencies_list.push_back(program_counter+2);
                    ST[SeparatedLine.argument2].is_def = 0;
                    //MANIPULA O CÓDIGO OBJETO   
                    
                    object_code.push_back(-1);
                }
                else if (ST[SeparatedLine.argument2].is_def)
                {
                    //MANIPULA O CÓDIGO OBJETO   
                    object_code.push_back(ST[SeparatedLine.argument2].address);
                }
                else
                {
                    ST[SeparatedLine.argument2].pendencies_list.push_back(program_counter+2);
                    //MANIPULA O CÓDIGO OBJETO  
                    object_code.push_back(-1);  
                }
            }

            else if(string::npos != SeparatedLine.argument2.find('+'))
            {
                divide_vector(SeparatedLine.argument2);
                
                if(!VerifyLabel(vectorization_aux[0], ST))
                {
                    ST[vectorization_aux[0]].pendencies_list.push_back(program_counter+2);
                    ST[vectorization_aux[0]].is_def = 0;
                    //MANIPULA O CÓDIGO OBJETO   
                    if(isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1-stoi(vectorization_aux[1])); 
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1);
                    }
                }
                else if (ST[vectorization_aux[0]].is_def)
                {
                    //MANIPULA O CÓDIGO OBJETO
                    if(isNumber(vectorization_aux[1]))
                    {   
                        object_code.push_back(ST[vectorization_aux[0]].address + stoi(vectorization_aux[1]));
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(ST[vectorization_aux[0]].address);
                    }
                }
                else
                {
                    ST[vectorization_aux[0]].pendencies_list.push_back(program_counter+2);
                    //MANIPULA O CÓDIGO OBJETO 
                    if(isNumber(vectorization_aux[1]))
                    { 
                        object_code.push_back(-1-stoi(vectorization_aux[1])); 
                    }
                    else if(!isNumber(vectorization_aux[1]))
                    {
                        object_code.push_back(-1);
                    }
                }
            }
        }
    }
}  

//Função que varre a lista de pendências e resolve ela no código objeto
void Solve_Pendency(map<string, table_item>& table)
{
    int aux_vector_pendency = 0;
    for (auto it: table)
    {
        for(int i = 0; i < it.second.pendencies_list.size(); i++)
        {
            
            if(object_code[it.second.pendencies_list[i]] == -1)
            {
            object_code[it.second.pendencies_list[i]] = it.second.address;
            }
            else if(object_code[it.second.pendencies_list[i]] < -1)
            {
                aux_vector_pendency = -1 - object_code[it.second.pendencies_list[i]];
                object_code[it.second.pendencies_list[i]] = it.second.address + aux_vector_pendency;
            }
        }
    }
    //VERIFICA DECLARAÇÃO DE rotulo AUSENTE
    for (auto it: table)
    {
        //se não está definido
        if (it.second.is_def == false)
        {
            error++;
            cout << "[Erro Semantico] - O rotulo " << it.first << " nao foi definido." << endl;
        }
    }
}

//Função pra verificar se é um vetor ou não 
void divide_vector(string argument)
{
    int v = 0;
    vectorization_aux[0] = "";
    vectorization_aux[1] = "";
    char ignore_plus_sign = '+';
    string vector_buffer = "";

    //Varre o argumento para dividir entre rotulo e offset
    for(auto each_char:argument)
    {
        if(each_char != ignore_plus_sign)
        {
            vector_buffer += each_char; 
        }
        if(each_char == ignore_plus_sign)
        {
            vectorization_aux[v] = vector_buffer;
            vector_buffer = "";
            v++;
        }
    }
    vectorization_aux[v] = vector_buffer;
}


void section_data_first()
{
    
    if (is_data_first == true)
    {
        
        text_sec_size = object_code.size() - data_sec_size;
        for (int q = 0; q < text_sec_size; q++)
        {
            if(is_opcode[q + data_sec_size] == true)
            {
                object_code_aux.push_back(object_code[q + data_sec_size]);
            }
            else if(is_opcode[q + data_sec_size] == false)
            {
                if(object_code[q + data_sec_size] <= data_sec_size)
                {
                    object_code_aux.push_back(object_code[q + data_sec_size] + text_sec_size);
                }
                else if(object_code[q + data_sec_size] >= data_sec_size)
                {
                    object_code_aux.push_back(object_code[q + data_sec_size] - data_sec_size);
                }
            }
        }
        for (int g = 0; g < data_sec_size ; g++)
        {
            object_code_aux.push_back(object_code[g]);
        }
    }
}

//contar o tamanho da seção de dados e de texto de text
void Parser(code_line SeparatedLine, int current_line)
{
    //Para o Label
    if (!SeparatedLine.label.empty())
    {
        if (SeparatedLine.label.size() > 50)
        {
            cout << "[Erro Lexico na linha " << current_line << "] - Rotulo " << SeparatedLine.label << " ultrapassa o limite de 50 caracteres." << endl;
            error++;
        }
        //Testa se primeiro caractere de um rotulo é número
        if (isdigit(SeparatedLine.label[0]))
        {
            cout << "[Erro Lexico na linha " << current_line << "] - Primeiro caractere do rotulo " << SeparatedLine.label << " eh um numero." << endl;
            error++;
        }
        //Testa se a label só possui caracteres válidos
        string::size_type invalid_char = SeparatedLine.label.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789_");
        if(invalid_char!=string::npos)
        {
            cout << "[Erro Lexico na linha " << current_line << "] - O rotulo " << SeparatedLine.label << " possui caractere(s) invalido(s)." << endl;
            error++;
        }
    }
    //Para o caso de mais de um label na mesma LINHA  more_than_one_label
    if (!more_than_one_label[0].empty())
    {
        if (more_than_one_label[0].size() > 50)
        {
            cout << "[Erro Lexico na linha " << current_line << "] - Rotulo "<< more_than_one_label[0] << " ultrapassa o limite de 50 caracteres." << endl;
            error++;
        }
        //Testa se primeiro caractere de um rotulo é número
        if (isdigit(more_than_one_label[0][0]))
        {
            cout << "[Erro Lexico na linha " << current_line << "] - Primeiro caractere do rotulo "<< more_than_one_label[0] << " eh um numero." << endl;
            error++; 
        }
        //Testa se a label só possui caracteres válidos
        string::size_type invalid_char = more_than_one_label[0].find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789_");
        if(invalid_char!=string::npos)
        {
            cout << "[Erro Lexico na linha " << current_line << "] - O rotulo "<< more_than_one_label[0] << " possui caractere(s) invalido(s)." << endl;
            error++;
        }
    }
    //Para o caso de 3 rotulos na mesma linha
        if (!more_than_one_label[1].empty())
    {
        if (more_than_one_label[1].size() > 50)
        {
            cout << "[Erro Lexico na linha " << current_line << "] - Rotulo "<< more_than_one_label[1] << " ultrapassa o limite de 50 caracteres." << endl;
            error++;
        }
        //Testa se primeiro caractere de um rotulo é número
        if (isdigit(more_than_one_label[1][0]))
        {
            cout << "[Erro Lexico na linha " << current_line << "] - Primeiro caractere do rotulo "<< more_than_one_label[1] << " eh um numero." << endl;
            error++;
        }
        //Testa se a label só possui caracteres válidos
        string::size_type invalid_char = more_than_one_label[1].find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789_");
        if(invalid_char!=string::npos)
        {
            cout << "[Erro Lexico na linha " << current_line << "] - O rotulo "<< more_than_one_label[1] << " possui caractere(s) invalido(s)." << endl;
            error++;
        }
    }


    //Para a operação
    if (!SeparatedLine.operation.empty())
    {
        if (SeparatedLine.operation.size() > 50)
        {
            cout << "[Erro Lexico na linha " << current_line << "] - Operacao " << SeparatedLine.operation << " ultrapassa o limite de 50 caracteres." << endl;
            error++;
        }
        //Testa se primeiro caractere de um rotulo é número
        if (isdigit(SeparatedLine.operation[0]))
        {
            cout << "[Erro Lexico na linha " << current_line << "] - Primeiro caractere da operação " << SeparatedLine.operation << " eh um numero." << endl;
            error++;
        }
        //Testa se a label só possui caracteres válidos
        string::size_type invalid_char = SeparatedLine.operation.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789_");
        if(invalid_char!=string::npos)
        {
            cout << "[Erro Lexico na linha " << current_line << "] - A operacao " << SeparatedLine.operation << " possui caractere(s) invalido(s)." << endl;
            error++;
        }
    }

    //Para o argumento 1
    if (!SeparatedLine.argument1.empty())
    {
        //TESTE PARA SPACE E CONST
        if(SeparatedLine.operation == "SPACE" || SeparatedLine.operation == "CONST")
        {
            if (!isNumber(SeparatedLine.argument1))
                {
                    cout << "[Erro Lexico na linha " << current_line << "] - O argumento da operacao " << SeparatedLine.operation << " nao eh um inteiro positivo. (" << SeparatedLine.argument1 << ")." << endl;
                    error++;
                }
        }
        else
        {
            if(string::npos == SeparatedLine.argument1.find('+'))
            {
                if (SeparatedLine.argument1.size() > 50)
                {
                    cout << "[Erro Lexico na linha " << current_line << "] - Rotulo " << SeparatedLine.argument1 << " ultrapassa o limite de 50 caracteres." << endl;
                    error++;
                }
                //Testa se primeiro caractere de um rotulo é número
                if (isdigit(SeparatedLine.argument1[0]))
                {
                    cout << "[Erro Lexico na linha " << current_line << "] - Primeiro caractere do rotulo " << SeparatedLine.argument1 << " eh um numero." << endl;
                    error++;
                }
                //Testa se a label só possui caracteres válidos
                string::size_type invalid_char = SeparatedLine.argument1.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789_");
                if(invalid_char!=string::npos)
                {
                    cout << "[Erro Lexico na linha " << current_line << "] - O rotulo " << SeparatedLine.argument1 << " possui caractere(s) invalido(s)." << endl;
                    error++;
                }
            }
            else if(string::npos != SeparatedLine.argument1.find('+'))
            {
                divide_vector(SeparatedLine.argument1);
                if (vectorization_aux[0].size() > 50)
                {
                    cout << "[Erro Lexico na linha " << current_line << "] - Rotulo " << vectorization_aux[0] << " ultrapassa o limite de 50 caracteres." << endl;
                    error++;
                }
                //Testa se primeiro caractere de um rotulo é número
                if (isdigit(vectorization_aux[0][0]))
                {
                    cout << "[Erro Lexico na linha " << current_line << "] - Primeiro caractere do rotulo " << vectorization_aux[0] << " eh um numero." << endl;
                    error++;
                }
                //Testa se a label só possui caracteres válidos
                string::size_type invalid_char = vectorization_aux[0].find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789_");
                if(invalid_char!=string::npos)
                {
                    cout << "[Erro Lexico na linha " << current_line << "] - O rotulo " << vectorization_aux[0] << " possui caractere(s) invalido(s)." << endl;
                    error++;
                }
                //Testa se vectorization_aux é número
                if (!isNumber(vectorization_aux[1]))
                {
                    cout << "[Erro Lexico na linha " << current_line << "] - Parametro " << vectorization_aux[1] << "  no vetor nao eh um inteiro." << endl;
                    error++;
                }
            }
        }
    }
    //Para o argumento 2
    
    
    if (!SeparatedLine.argument2.empty())
    {
        if(string::npos == SeparatedLine.argument2.find('+'))
        {
            if (SeparatedLine.argument2.size() > 50)
            {
                cout << "[Erro Lexico na linha " << current_line << "] - Rotulo " << SeparatedLine.argument2 << " ultrapassa o limite de 50 caracteres." << endl;
                error++;
            }
            //Testa se primeiro caractere de um rotulo é número
            if (isdigit(SeparatedLine.argument2[0]))
            {
                cout << "[Erro Lexico na linha " << current_line << "] - Primeiro caractere do rotulo " << SeparatedLine.argument2 << " eh um numero." << endl;
                error++;
            }
            //Testa se a label só possui caracteres válidos
            string::size_type invalid_char = SeparatedLine.argument2.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789_");
            if(invalid_char!=string::npos)
            {
                cout << "[Erro Lexico na linha " << current_line << "] - O rotulo " << SeparatedLine.argument2 << " possui caractere(s) invalido(s)." << endl;
                error++;
            }
        }
        else if(string::npos != SeparatedLine.argument2.find('+'))
        {
            divide_vector(SeparatedLine.argument2);
            if (vectorization_aux[0].size() > 50)
            {
                cout << "[Erro Lexico na linha " << current_line << "] - Rotulo " << vectorization_aux[0] << " ultrapassa o limite de 50 caracteres." << endl;
                error++;
            }
            //Testa se primeiro caractere de um rotulo é número
            if (isdigit(vectorization_aux[0][0]))
            {
                cout << "[Erro Lexico na linha " << current_line << "] - Primeiro caractere do rotulo " << vectorization_aux[0] << " eh um numero." << endl;
                error++;
            }
            //Testa se a label só possui caracteres válidos
            string::size_type invalid_char = vectorization_aux[0].find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789_");
            if(invalid_char!=string::npos)
            {
                cout << "[Erro Lexico na linha " << current_line << "] - O rotulo " << vectorization_aux[0] << " possui caractere(s) invalido(s)." << endl;
                error++;
            }
            //Testa se vectorization_aux é número
            if (!isNumber(vectorization_aux[1]))
            {
                cout << "[Erro Lexico na linha " << current_line << "] - Parametro " << vectorization_aux[1] << " no vetor nao eh um inteiro." << endl;
                error++;
            }
        }
    }

    //Se tem mais de 2 argumentos
    if (!more_than_2_arguments.empty())
    {
        if(string::npos == more_than_2_arguments.find('+'))
        {
            if (more_than_2_arguments.size() > 50)
            {
                cout << "[Erro Lexico na linha " << current_line << "] - Rotulo " << more_than_2_arguments << " ultrapassa o limite de 50 caracteres." << endl;
                error++;
            }
            //Testa se primeiro caractere de um rotulo é número
            if (isdigit(more_than_2_arguments[0]))
            {
                cout << "[Erro Lexico na linha " << current_line << "] - Primeiro caractere do rotulo " << more_than_2_arguments << " eh um numero." << endl;
                error++;
            }
            //Testa se a label só possui caracteres válidos
            string::size_type invalid_char = more_than_2_arguments.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789_");
            if(invalid_char!=string::npos)
            {
                cout << "[Erro Lexico na linha " << current_line << "] - O rotulo " << more_than_2_arguments << " possui caractere(s) invalido(s)." << endl;
                error++;
            }
        }
        else if(string::npos != more_than_2_arguments.find('+'))
        {
            divide_vector(more_than_2_arguments);
            if (vectorization_aux[0].size() > 50)
            {
                cout << "[Erro Lexico na linha " << current_line << "] - Rotulo " << vectorization_aux[0] << " ultrapassa o limite de 50 caracteres." << endl;
                error++;
            }
            //Testa se primeiro caractere de um rotulo é número
            if (isdigit(vectorization_aux[0][0]))
            {
                cout << "[Erro Lexico na linha " << current_line << "] - Primeiro caractere do rotulo " << vectorization_aux[0] << " eh um numero." << endl;
                error++;
            }
            //Testa se a label só possui caracteres válidos
            string::size_type invalid_char = vectorization_aux[0].find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789_");
            if(invalid_char!=string::npos)
            {
                cout << "[Erro Lexico na linha " << current_line << "] - O rotulo " << vectorization_aux[0] << " possui caractere(s) invalido(s)." << endl;
                error++;
            }
            //Testa se vectorization_aux é número
            if (!isNumber(vectorization_aux[1]))
            {
                cout << "[Erro Lexico na linha " << current_line << "] - Parametro " << vectorization_aux[1] << " no vetor nao eh um inteiro." << endl;
                error++;
            }
        }
    }
}

bool isNumber(const string& str)
{
    for (char const &c : str) {
        if (std::isdigit(c) == 0) return false;
    }
    return true;
}