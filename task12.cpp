#include <iostream>
#include <cmath>
#include <iomanip>

//#define DEBUGINFO

template<typename type>
class vec2{
private:
    type x;
    type y;
public:
    vec2():x(0),y(0){}
    vec2(type x, type y):x(x),y(y){};
    vec2(const vec2<type> &arg){x=arg.x;y=arg.y;}

    vec2<type> operator+(const vec2<type> &arg) const{return vec2<type>(x+arg.x,y+arg.y);}
    vec2<type> operator-(const vec2<type> &arg) const{return vec2<type>(x-arg.x,y-arg.y);}
    vec2<type> operator*(type &t)               const{return vec2<type>(x*t,y*t);}
    vec2<type> & operator=(const vec2<type> &arg)    {x=arg.x; y=arg.y; return *this;}
    friend type module(const vec2<type> &arg)        {return std::sqrt(arg.x*arg.x+arg.y*arg.y);}
    friend std::ostream & operator<<(std::ostream & out,const vec2<type> &arg){out<<arg.x<<'\t'<<arg.y; return out;}
};

template<typename type>
class manipulator{
private:
    vec2<type>  O;
    vec2<type>  hand;
    type        R;
public:
    manipulator():O(0),R(0){}
    manipulator(const vec2<type> &O, const type &R):O(O),hand(O),R(R){}
    manipulator(const vec2<type> &O, const vec2<type> &hand, const type &R):O(O),hand(hand),R(R){}
    manipulator(const manipulator &m){O(m.o);R=m.R;hand(m.hand);}

    type getR()                             const{return R;}
    type distansTo(const vec2<type> &point) const{return module(hand-point);}
    vec2<type> currentHandPosition()        const{return hand;}
    vec2<type> currentPosition()            const{return O;}
    void moveHandTo(const vec2<type> &point){hand=point;}
    void moveTo(const vec2<type> &point)    {O=point;}
};

template<typename type>
vec2<type> linearInterpolation(const vec2<type> &v1, const vec2<type> &v2, type t){
    t /= module(v1-v2);                                                     //величина параметра t нормируется на единицу
    return vec2<type>( v1 + (v2-v1)*t );                                    //без нормировки это уравнение выглядит сложнее
}

void task12()
{
    manipulator<double> M1({0.0,0.0},4.0);                                  //определяем манипулятор 1
    manipulator<double> M2({2.0,1.0},3.0);                                  //и манипулятор 2

    vec2<double> point[5] = {                                               //точки по условию
        {1.0,3.0},{2.0,1.41},{0.2,-7.0},{-5.0,-1.0},{0.0,9.0}
    };

    //задача1 (на примере данных из второй задачи разберём первую задачу )
    std::cout<<"task1"<<std::endl;
    double dist1 = M1.distansTo(point[0]);                                  //измеряем дистанцию от конца руки манипулятора 1 до точки
    double dist2 = M2.distansTo(point[0]);                                  //аналогично для манипулятора 2

    if(dist1<=dist2){                                                       //если конец манипулятора 1 оказался ближе чем манипулятор 2
        M1.moveHandTo(point[0]);                                            //передвигаем руку манипулятора в эту точку
        if(dist1>M1.getR()){                                                //если же рука не достаёт до точки, то мы должны одновременно переместить точку O
            vec2<double> O = M1.currentPosition();                          //в новую точку, чтобы рука манипулятора достала до этой точки
            vec2<double> H = M1.currentHandPosition();                      //предполагается что точку O манипулятора можно двигать и скорость её перемещения равна скорости руки
            M1.moveTo(linearInterpolation(O,H,module(O-H)-M1.getR()));      //тогда между начальной точкой O и конечной точкой руки H проводим линейную интерполяцию в точку с параметром |O-H|-R
        }
        std::cout<<"M1 is optimal"<<std::endl;
    }else{                                                                  //аналогично для манипулятора 2
        M2.moveHandTo(point[0]);
        if(dist2>M2.getR()){
            vec2<double> O = M2.currentPosition();
            vec2<double> H = M2.currentHandPosition();
            M2.moveTo(linearInterpolation(O,H,module(O-H)-M2.getR()));
        }
        std::cout<<"M2 is optimal"<<std::endl;
    }
    std::cout<<std::endl;

    //задача2
    std::cout<<"task2"<<std::endl;
    vec2<double> *M1points[5];                                              //указатели на пройденные точки манипулятором 1
    vec2<double> *M2points[5];                                              //указатели на пройденные точки манипулятором 2

    for(size_t i=0;i<5;i++){
        double dist1 = M1.distansTo(point[i]);
        double dist2 = M2.distansTo(point[i]);

        if(dist1<=dist2){
            M1.moveHandTo(point[i]);
            if(dist1>M1.getR()){
                vec2<double> O = M1.currentPosition();
                vec2<double> H = M1.currentHandPosition();
                M1.moveTo(linearInterpolation(O,H,module(O-H)-M1.getR()));
            }
            M1points[i] = &point[i];
            M2points[i] = nullptr;
        }else{
            M2.moveHandTo(point[i]);
            if(dist2>M2.getR()){
                vec2<double> O = M2.currentPosition();
                vec2<double> H = M2.currentHandPosition();
                M2.moveTo(linearInterpolation(O,H,module(O-H)-M2.getR()));
            }
            M1points[i] = nullptr;
            M2points[i] = &point[i];
        }

        #ifdef DEBUGINFO
            std::cout<<"point"<<'\t'<<"px"<<'\t'<<"py"<<std::endl;
            std::cout<<'\t'<<std::setprecision(2)<<point[i]<<std::endl;
            std::cout<<'\t'<<"Ox"<<'\t'<<"Oy"<<'\t'<<"handx"<<'\t'<<"handy"<<'\t'<<"dist"<<std::endl;
            std::cout<<"M1"<<'\t'<<std::setprecision(2)<<M1.currentPosition()<<'\t'<<M1.currentHandPosition()<<'\t'<<dist1<<std::endl;
            std::cout<<"M2"<<'\t'<<std::setprecision(2)<<M2.currentPosition()<<'\t'<<M2.currentHandPosition()<<'\t'<<dist2<<std::endl;
            std::cout<<"================================"<<std::endl;
        #endif
    }

    //вывод таблицы
    std::cout<<"\t"<<"iter1\t\t"<<"iter2\t\t"<<"iter3\t\t"<<"iter4\t\t"<<"iter5\t\t"<<std::endl;

    std::cout<<"M1\t";
    for(size_t i=0;i<5;i++){
        if(M1points[i]!=nullptr)    std::cout<<"{"<<*M1points[i]<<"}"<<'\t';
        else                        std::cout<<'\t'<<'\t';
    }std::cout<<std::endl;

    std::cout<<"M2\t";
    for(size_t i=0;i<5;i++){
        if(M2points[i]!=nullptr)    std::cout<<"{"<<*M2points[i]<<"}"<<'\t';
        else                        std::cout<<'\t'<<'\t';
    }std::cout<<std::endl<<std::endl;
}
