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
#include <limits>

using namespace std;

//Map para a Section Data
//map<int,int> SectionData;

//Map para a Section Text
//map<int,int> SectionText;

void Simulator(string ObjectName);
//void SeparateSection(string OutputFileLine);
void Build_Vector(string OutputFileLine);
void Build_Line_Counter(string buffer);
vector<int> OutputVector;
map <int,int> code_line_map;
int aux = 0;
int cont_end = 0;
int aux_cont_end = 0;
int code_line = 1;
bool aux_1 = true;

int main(int argc, char **argv)
{
    string ObjectLine, ObjectName = argv[1]; //verificar necessidade da variável line
    int ObjectSize = ObjectName.size();

    if(ObjectName.substr(ObjectSize - 4, ObjectSize) == ".obj")
    { 
        Simulator(ObjectName); 
        return 0;
    }
    else
    {
        cout << "[Error] O arquivo selecionado não é compatível." << endl;
        return 1;
    }
}

void Simulator(string ObjectName)
{
    int ACC = 0;
    int PC = 0;
    int output = 0;
    int line_counter = 1;
    int current_line = 0;
    string OutputFileLine;
    //abre o arquivo .obj, que é a entrada do simulador
    ifstream input_file_obj;
    //abre o arquivo .out, que é a saída do simulador
    //ofstream output_file_obj;

    //Abrir arquivo .obj (pode fazer input_file_obj.open("Output/" + ObjectName); para escolher outra pasta para o arquivo objeto)
    input_file_obj.open(ObjectName);
    //Abrir arquivo de saída do simulador .out
    //output_file_obj.open("output.out");
    //Recebe a única linh do arquivo .obj
    getline(input_file_obj,OutputFileLine);
    //Chama a função para preencher as tabelas de dados da Section Text e Section Data
    //SeparateSection(OutputFileLine);
    Build_Vector(OutputFileLine);



    //O codigo deve rodar até a instrucao STOP (opcode 14) aparecer
    //Para rodar, usaremos o contador de programa para ir avançando as instruções
    
    while (OutputVector[PC] != 14)
    {   
        //Farei um switch case para cada instrucao, de modo que permita atualizar no terminal os valores do acumulador e do contador de programa
        switch (OutputVector[PC])
        {
            case 1:
                cout << "Linha " << code_line_map[PC] << " do codigo - Instrucao de numero " << line_counter << " - Apos a instrucao ADD:" << endl;
                ACC = ACC + OutputVector[OutputVector[PC + 1]];
                PC += 2;
                line_counter++;
                break;

            case 2:
                cout << "Linha " << code_line_map[PC] << " do codigo - Instrucao de numero " << line_counter << " - Apos a instrucao SUB:" << endl;
                ACC = ACC - OutputVector[OutputVector[PC + 1]];
                PC += 2;
                line_counter++;
                break;

            case 3:
                cout << "Linha " << code_line_map[PC] << " do codigo - Instrucao de numero " << line_counter << " - Apos a instrucao MUL:" << endl;
                ACC = ACC * OutputVector[OutputVector[PC + 1]];
                PC += 2;
                line_counter++;
                break;

            case 4:
                cout << "Linha " << code_line_map[PC] << " do codigo - Instrucao de numero " << line_counter << " - Apos a instrucao DIV:" << endl;
                ACC = ACC / OutputVector[OutputVector[PC + 1]];
                PC += 2;
                line_counter++;
                break;
                
            case 5:
                cout << "Linha " << code_line_map[PC] << " do codigo - Instrucao de numero " << line_counter << " - Apos a instrucao JMP:" << endl;
                PC = OutputVector[PC + 1];
                line_counter++;

                break;

            case 6:
                cout << "Linha " << code_line_map[PC] << " do codigo - Instrucao de numero " << line_counter << " - Apos a instrucao JMPN:" << endl;
                if(ACC < 0){
                    PC = OutputVector[PC + 1];
                }
                else{
                    PC += 2;
                }
                line_counter++;
                break;

            case 7:
                cout << "Linha " << code_line_map[PC] << " do codigo - Instrucao de numero " << line_counter << " - Apos a instrucao JMPP:" << endl;
                if(ACC > 0){
                    PC = OutputVector[PC + 1];
                }
                else{
                    PC += 2;
                }
                line_counter++;
                break;

            case 8:
                cout << "Linha " << code_line_map[PC] << " do codigo - Instrucao de numero " << line_counter << " - Apos a instrucao JMPZ:" << endl;
                if(ACC == 0){
                    PC = OutputVector[PC + 1];
                }
                else{
                    PC += 2;
                }
                line_counter++;
                break;

            case 9:
                cout << "Linha " << code_line_map[PC] << " do codigo - Instrucao de numero " << line_counter << " - Apos a instrucao COPY:" << endl;
                OutputVector[OutputVector[PC + 2]] = OutputVector[OutputVector[PC + 1]];
                PC += 3;
                line_counter++;
                break;

            case 10:
                cout << "Linha " << code_line_map[PC] << " do codigo - Instrucao de numero " << line_counter << " - Apos a instrucao LOAD:" << endl;
                ACC = OutputVector[OutputVector[PC + 1]];
                PC += 2;
                line_counter++;
                break;

            case 11:
                cout << "Linha " << code_line_map[PC] << " do codigo - Instrucao de numero " << line_counter << " - Apos a instrucao STORE:" << endl;
                OutputVector[OutputVector[PC + 1]] = ACC;
                PC += 2;
                line_counter++;
                break;

            case 12:
                cout << "Linha " << code_line_map[PC] << " do codigo - Instrucao de numero " << line_counter << " - Insira um valor:" << endl;
                cin >> OutputVector[OutputVector[PC + 1]];
                PC += 2;
                line_counter++;
                break;

            case 13:
                
                
                output = OutputVector[OutputVector[PC + 1]];   
                cout <<"Output: " << output << endl;
                cout << "Press enter to Continue" << endl;
                //cin.ignore(numeric_limits<streamsize>::max(),'\n');      
                //cin.ignore();
                cin.clear();
                cin.get();
                cout << "Linha " << code_line_map[PC] << " do codigo - Instrucao de numero " << line_counter << " - Apos a instrucao OUTPUT:" << endl;       
                //output_file_obj << output;
                //output_file_obj << " ";
                PC += 2;
                line_counter++;
                break;

        }

        cout << "PC" << " <- "<< PC << endl;
        cout << "ACC" << " <- " << ACC << endl;

    }
    //Fecha os dois arquivos
    input_file_obj.close();
    //output_file_obj.close();
}


void Build_Vector(string OutputFileLine)
{
    // Delimitador para retirarmos o espaco da linh do opcode 
    char space = ' '; 
    // String buffer para acumular os numeros
    string buffer = "";
    int i = 0;

    for(auto letra:OutputFileLine)
	{
        // Armazenamos no buffer 
		if(letra != space) 
        {
            buffer+=letra; 
        }
        
        if(letra == space  && buffer != "")
        { 
            OutputVector.push_back(stoi(buffer));
            Build_Line_Counter(buffer);
            buffer = "";
            i++;
        }
    }
    if(!buffer.empty())
    {
        OutputVector.push_back(stoi(buffer));
    }
}

void Build_Line_Counter(string buffer)
{
    int opcode = stoi(buffer);
    
    if (opcode == 14)
    {   
        aux_1 = false;
    }    
    if(aux_1 == true)
    {
        if(aux == 0)
        {
            code_line++;
            code_line_map[cont_end] = code_line;

            switch (opcode)
            {
                case 1:
                    aux += 1;
                    cont_end += 2;
                    break;

                case 2:
                    aux += 1;
                    cont_end += 2;
                    break;

                case 3:
                    aux += 1;
                    cont_end += 2;
                    break;

                case 4:
                    aux += 1;
                    cont_end += 2;
                    break;
                    
                case 5:
                    aux += 1;
                    cont_end += 2;

                    break;

                case 6:
                    aux += 1;
                    cont_end += 2;
                    break;

                case 7: 
                    aux += 1;
                    cont_end += 2;
                    break;

                case 8: 
                    aux += 1;
                    cont_end += 2;
                    break;

                case 9: 
                    aux += 2;
                    cont_end += 3;
                    break;

                case 10: 
                    aux += 1;
                    cont_end += 2;
                    break;

                case 11: 
                    aux += 1;
                    cont_end += 2;
                    break;

                case 12: 
                    aux += 1;
                    cont_end += 2;
                    break;

                case 13: 
                    aux += 1;
                    cont_end += 2;
                    break;
            }
        }  
        else
        {
            aux--;
        }  
    }
}