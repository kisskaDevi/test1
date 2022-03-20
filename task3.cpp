#include <iostream>
#include <vector>
#include <string>

enum result{
    success = 0,
    numberError = 1,
    endFile = 2,
    incorrectAmountOfData = 3,
    incorrectCommand = 4
};

void printDebugMassage(result res){
    switch (res) {
    case result::success:
        break;
    case result::numberError:
        std::cout<<"number reading error"<<std::endl;
        break;
    case result::endFile:
        break;
    case result::incorrectAmountOfData:
        std::cout<<"incorrect amount of data in string"<<std::endl;
        break;
    case result::incorrectCommand:
        std::cout<<"command reading error"<<std::endl;
        break;
    }
}

int isNumber(const char &byte){
    switch (byte) {
    case 48:
        return 0;
    case 49:
        return 1;
    case 50:
        return 2;
    case 51:
        return 3;
    case 52:
        return 4;
    case 53:
        return 5;
    case 54:
        return 6;
    case 55:
        return 7;
    case 56:
        return 8;
    case 57:
        return 9;
    case 45:
        return -1;
    }
    return -3;
}

int isCommand(const char &byte){
    switch (byte) {
    case 88:
        return 0;
    case 89:
        return 1;
    case 90:
        return 2;
    }
    return -3;
}

result readFirstString(FILE *GCODE, std::vector<int> &coord)                        //функция чтения первой строки выполняет функцию
{                                                                                   //записи начальных данных в переменную coord
    result res = result::success;

    coord.resize(3);
    std::vector<std::string> buffer(1);                                             //строки в которые будем записывать цыфры
    char byte;
    while((byte=getc(GCODE))!= '\n'&& byte != EOF){
        if(byte!=' '){                                                              //если символ не пробел
            if(isNumber(byte)!=-3){                                                     //если символ byte является числом
                if(isNumber(byte)!=-1)                                                      //проверяем не является ли символ знаком "минус"
                    buffer[buffer.size()-1] += byte;                                            //добавляем символ
                else if(isNumber(byte)==-1&&buffer[buffer.size()-1].size()==0)              //если минус то проверяем не стоит ли он на первом месте в строке, что допустимо
                    buffer[buffer.size()-1] += byte;                                            //добавляем символ
                else{                                                                       //в противном случае допущена ошибка в записи числа
                    res = result::numberError;                                                  //записываем в переменную res, что ошибка в записи числа
                    break;                                                                      //прерываем цикл
                }
            }
        }else                                                                       //если символ пробел
            buffer.resize(buffer.size()+1);                                             //добавляем новую строку для следующего числа
    }

    if(res == result::success){

        if(byte == EOF)
            res = result::endFile;
        else{
            if(buffer.size()==3)
                for(size_t i=0;i<3;i++)
                    coord[i] = std::stoi(buffer[i]);            //переводим string в int
            else
                res = result::incorrectAmountOfData;            //если записана строка в которой больше чисел чем нужно, например "10 10 20 30", то выводится эта ошибка
        }
    }

    return res;
}

result readString(FILE *GCODE, std::vector<int> &coord)     //функция для чтения непервой сторки
{
    result res = result::success;

    coord.resize(3);
    std::vector<std::string> buffer(1);
    char byte;
    int command = 0;                                                                                                        //переменная для проверки правильности записи команд
    while((byte=getc(GCODE))!= '\n' && byte != EOF){
        if(byte!=';'){
            if(buffer[buffer.size()-1].size()==0){                                                                          //если буфер пустой, то должна быть первая команда
                if(isCommand(byte)==command)        {command++;  buffer[buffer.size()-1] += byte;}                              //если комманда корректная, то записываем её в буфер и прибавляем 1 к переменной command для чтения следующей команды
                else                                {res = result::incorrectCommand; break;}                                    //иначе ошибка в записи команды
            }else{                                                                                                          //если команда записана то должна быть цифра
                if(isNumber(byte)!=-3){                                                                                         //чтение цифр такое же как и в первой строке
                    if(isNumber(byte)!=-1)                                          buffer[buffer.size()-1] += byte;
                    else if(isNumber(byte)==-1&&buffer[buffer.size()-1].size()==1)  buffer[buffer.size()-1] += byte;
                    else{                                                           res = result::numberError;  break;}
                }
            }
        }else{                                                                                                              //если символ ';' то добавляем новую строку для следующего числа
            buffer.resize(buffer.size()+1);
        }
    }

    if(res == result::success){                                             //если не было ошибок то можно записывать результат в coord
        if(byte == EOF){
            res = result::endFile;
            buffer.erase(buffer.end()-1,buffer.end());
        }else{
            if(byte =='\n')                                                 //в файле в конце строки пишется ';' из-за чего образуется лишняя строка
                buffer.erase(buffer.end()-1,buffer.end());                  //поэтому её надо удалить
            if(buffer.size()==3)
                for(size_t i=0;i<3;i++){
                    buffer[i].erase(buffer[i].begin(),buffer[i].begin()+1); //начальный символ это команда 'X', 'Y' или 'Z', это не число и его тоже удаляем
                    coord[i] += std::stoi(buffer[i]);                       //переводим string в int
                }
            else
                res = result::incorrectAmountOfData;
        }
    }

    return res;
}

void task3(){
    //задача3
    std::cout<<"task3"<<std::endl;

    std::vector<int> coord;

    result res;

    FILE *GCODE = fopen("C:\\Users\\kiril\\OneDrive\\qt\\test1\\ex1", "r");
    res = readFirstString(GCODE, coord);        //читаем сначала первую строку
    if(res==result::success){
        while (res == result::success) {        //затем считываем пока не встретим ошибку или конец файла
            res = readString(GCODE, coord);
        }
    }
    fclose (GCODE);

    std::cout<<coord.at(0)<<'\t';
    std::cout<<coord.at(1)<<'\t';
    std::cout<<coord.at(2)<<std::endl;

    printDebugMassage(res);                     //сообщение об ошибке
}
