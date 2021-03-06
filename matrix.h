#pragma once
#include <string>
#include <tuple>
#include "exception.h"
#include "GF.h"
#include <iostream>
#include <functional>

class vector
{
private:
        unsigned int l;

        void malloc()
        {
            this->v = new double[l];
        }

        void copy(const vector& m2)
        {
            this->l = m2.l;
            this->malloc();
            for (unsigned int i = 0; i < l; i++)
                this->v[i] = m2.v[i];
        }

        void free() { delete[]v; }

public:
        double *v;

        static vector one(uint l, uint pos)
        {
            vector retn(l);
            for(uint i=0;i<retn.getl();i++)
            {
                if(i==pos)
                    retn.v[pos] = 1;
                else
                    retn.v[i] = 0;
            }
            return retn;
        }

        vector(unsigned int l) : l(l)
        {
            this->malloc();
            for (unsigned int i = 0; i < l; i++)
                this->v[i] = 0;
        }

        vector(uint l, const std::function<double(uint)>& f)
        {
            this->l = l;
            this->malloc();
            for (uint i = 0; i < this->l; i++)
                this->v[i] = f(i);
        }

        vector(const vector& m2)
        {
            this->copy(m2);
        }

        vector& operator=(const vector& m2)
        {
            this->free();
            this->copy(m2);
            return *this;
        }

        double operator[](const uint i) const
        {
            return this->v[i];
        }

        double dot(const vector &v2) const
        {
                if (v2.l != this->l)
                    throw DimensionException();
                else
                {
                    double result = 0;
                    for (unsigned int i = 0; i < l; i++)
                    {
                        double m=GF::mul(this->v[i],v2.v[i]);
                        result=GF::add(result,m);
                    }
                    return result;
                }
        }

        vector mul(double n) const
        {
            vector result(this->l);
            for (unsigned int i = 0; i < l; i++)
                result.v[i]=GF::mul(this->v[i],n);
            return result;
        }

        vector add(vector v2) const
        {
            if(v2.l!=this->l)
                throw DimensionException();
            else
            {
                vector result(this->l);
                for (unsigned int i = 0; i < l; i++)
                    result.v[i]=GF::add(this->v[i],v2.v[i]);
                return result;
            }
        }

        double norm(unsigned int p) const
        {
            double ans = 0;
            for (unsigned int i = 0; i < this->l; i++)
                ans += pow(abs(this->v[i]), p);
            return pow(ans, 1.0 / p);
        }

        void output() const
        {
            for (unsigned int i = 0; i < l; i++)
            {
                printf("%g ", v[i]);
            }
            printf("\n");
        }

        ~vector() { this->free(); }
        unsigned int getl() const { return l; }
};


class matrix
{
private:
        unsigned int r;
        unsigned int c;
        const double Err = 1e-5;

        void malloc()
        {
                this->m = new double*[r]; //给第一维分配空间
                for (unsigned int i = 0; i < r; i++)
                        this->m[i] = new double[c]; //给第二维分配空间
        }

        bool checkZero(unsigned int pos) const
        {
            for (unsigned int i = 0; i < this->c; i++)
                if (abs(this->m[pos][i]) < this->Err) //如果小于误差就当做0
                    return false;
            return true;
        }

        void free()
        {
            for (unsigned int i = 0; i < r; i++)
                    delete[] m[i];
            delete[] m;
        }

        void copy(const matrix& m2)
        {
            this->r = m2.r;
            this->c = m2.c;
            this->malloc();
            for (unsigned int i = 0; i < r; i++)
            {
                for (unsigned int j = 0; j < c; j++)
                    this->m[i][j] = m2.m[i][j];
            }
        }

        vector solveWithLUP(const matrix& L, const matrix& U, const vector& P, const vector& b) const;


public:
        std::tuple<matrix,matrix,vector> LUPVec() const;
        double **m;

        matrix(unsigned int r, unsigned int c) : r(r), c(c)
        {
            this->malloc();
            //初始化为零矩阵
            for (unsigned int i = 0; i < r; i++)
            {
                    for (unsigned int j = 0; j < c; j++)
                            this->m[i][j] = 0;
            }
        }

        matrix(const vector& v, bool isR)
        {
            if (isR)
            {
                this->r = v.getl();
                this->c = 1;
            }
            else
            {
                this->r = 1;
                this->c = v.getl();
            }

            this->malloc();
            if (isR)
                for (uint i = 0; i < this->r; i++)
                    this->m[i][0] = v[i];
            else
                for (uint i = 0; i < this->c; i++)
                    this->m[0][i] = v[i];
        }

        matrix(uint r, uint c, const std::function<double(uint, uint)>& f)
        {
            this->r = r;
            this->c = c;
            this->malloc();
            for (uint i = 0; i < this->r; i++)
                for (uint j = 0; j < this->c; j++)
                    this->m[i][j] = f(i, j);
        }

        matrix(const matrix& m2)
        {
            this->copy(m2);
        }

        ~matrix()
        {
            this->free();
        }

        matrix& operator=(const matrix& m2)
        {
            this->free();
            this->copy(m2);
            return *this;
        }

        unsigned int getr() const { return r; }
        unsigned int getc() const { return c; }

        vector getRVector(unsigned int rn) const
        {
                vector result = vector(this->c);
                for (unsigned int i = 0; i < this->c; i++)
                        result.v[i] = this->m[rn][i];
                return result;
        }

        vector getCVector(unsigned int cn) const
        {
                vector result = vector(this->r);
                for (unsigned int i = 0; i < this->r; i++)
                        result.v[i] = this->m[i][cn];
                return result;
        }

        void setRVector(vector v, unsigned int rn)
        {
            if(v.getl()!=this->c)
                throw DimensionException();
            else
            {
                for (unsigned int i = 0; i < this->c; i++)
                    this->m[rn][i]=v.v[i];
            }
        }

        void setCVector(vector v, unsigned int cn)
        {
            if(v.getl()!=this->r)
                throw DimensionException();
            else
            {
                for (unsigned int i = 0; i < this->r; i++)
                    this->m[i][cn]=v.v[i];
            }
        }

        double algCofactor(unsigned int i,unsigned int j) const
        {
            matrix mb = matrix(r - 1, r - 1);
            for (unsigned int r = 0; r < this->r; r++)
            {
                for (unsigned int c = 0; c < this->c; c++)
                {
                    if(c==j || r==i)
                        continue;

                    int subR,subC;
                    if (c > j)
                        subC = c - 1;
                    else
                        subC = c;
                    if(r > i)
                        subR = r - 1;
                    else
                        subR = r;

                    mb.m[subR][subC] = this->m[r][c];
                }
            }
            //return pow(-1, i + j) * mb.det(); //fix:二进制的正负号取消
            return mb.det();
        }

        double det() const
        {
            if (this->r != this->c)
                throw SquareException();
            else if (this->r == 1)
                return this->m[0][0];
            else
            {
                double result = 0;
                //得到从当前矩阵中划去第0行和第j列的所有元素后得到的矩阵
                for (unsigned int j = 0; j < this->r; j++)
                {
                    double m = GF::mul(this->algCofactor(0,j),this->m[0][j]);
                    result = GF::add(result,m);
                }
                return result;
            }
        }

        matrix delC(uint ci1, uint ci2)
        {
            matrix m(this->r,this->c-2);
            uint findNum=0;
            for (unsigned int j = 0; j < c-2; j++)
            {
                if(j==ci1 || j+1==ci2)
                    findNum++;
                for (unsigned int i = 0; i < r; i++)
                    m.m[i][j] = this->m[i][j+findNum];
            }
            return m;
        }

        matrix adjoint() const
        {
            if (this->r != this->c)
                throw SquareException();
            else
            {
                matrix result = matrix(this->r,this->c);
                for (unsigned int i = 0; i < this->r; i++)
                {
                    for (unsigned int j = 0; j < this->c; j++)
                        result.m[i][j]=this->algCofactor(j,i);
                }
                return result;
            }
        }

        matrix cut(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
        {
            matrix ret(y2-y1+1,x2-x1+1);
            for(unsigned int y=y1 , i=0 ; y<=y2 ; y++ , i++)
            {
                for(unsigned int x=x1 , j=0 ; x<=x2 ; x++ , j++)
                    ret.m[i][j]=this->m[y][x];
            }
            return ret;
        }

        void setArea(unsigned int y1,unsigned int x1,unsigned int y2,unsigned int x2,
                       const matrix &m)
        {
            for(unsigned int x=0;x+x1<x2;x++)
            {
                for(unsigned int y=0;y+y1<y2;y++)
                    this->m[y+y1][x+x1]=m.m[y][x];
            }
        }

        matrix inv() const
        {
            if(this->r != this->c)
                throw SingularException();
            unsigned int n = this->r;
            matrix m = *this;
            matrix e = matrix::identity(n);

            // 变为右上三角矩阵
            for(unsigned int i = 0; i < n; i++)
            {
                unsigned int j;
                for(j = i; j < n; j++)
                    if(m.m[j][i] != 0)
                    {
                        e.rswap(i, j);
                        m.rswap(i, j);
                        break;
                    }
                if(j == n)
                    throw SingularException();
                //m.output();
                //std::cout<<std::endl;
                for(j = i + 1; j < n; j++)
                {
                    e.radd(j, i, GF::div(m.m[i][i],m.m[j][i]));
                    m.radd(j, i, GF::div(m.m[i][i],m.m[j][i]));
                }
            }

            // 变为单位矩阵
            for(unsigned int i = n - 1; i > 0; i--)
            {
                if(m.m[i][i] == 0)
                    throw SingularException();
                e.rmul(i, GF::mulInv(m.m[i][i]));
                m.rmul(i, GF::mulInv(m.m[i][i]));
                for(unsigned int j = i - 1; j > 0; j--)
                {
                    e.radd(j, i, GF::div(m.m[i][i],m.m[j][i]));
                    m.radd(j, i, GF::div(m.m[i][i],m.m[j][i]));
                }
                e.radd(0, i, GF::div(m.m[i][i],m.m[0][i]));
                m.radd(0, i, GF::div(m.m[i][i],m.m[0][i]));
            }
            e.rmul(0, GF::mulInv(m.m[0][0]));
            m.rmul(0, GF::mulInv(m.m[0][0]));
            return e;
        }

        matrix dot(const matrix &m2) const
        {
            if (this->c != m2.r)
                    throw DimensionException();
            else
            {
                matrix result = matrix(this->r, m2.c);
                for (unsigned int i = 0; i < this->r; i++)
                {
                        vector v1 = this->getRVector(i);
                        for (unsigned int j = 0; j < m2.c; j++)
                        {
                                vector v2 = m2.getCVector(j);
                                result.m[i][j] = v1.dot(v2);
                        }
                }
                return result;
            }
        }

        matrix mul(double n) const
        {
            matrix result(this->r,this->c);
            for (unsigned int i = 0; i < this->r; i++)
                for (unsigned int j = 0; j < this->c; j++)
                    result.m[i][j]=GF::mul(this->m[i][j],n);
            return result;
        }

        matrix add(matrix m2) const
        {
            if(m2.c==this->c&&m2.r==this->r)
            {
                matrix result(this->r,this->c);
                for (unsigned int i = 0; i < this->r; i++)
                    for (unsigned int j = 0; j < this->c; j++)
                        result.m[i][j]=GF::add(this->m[i][j],m2.m[i][j]);
                return result;
            }
            else
                throw DimensionException();
        }

        matrix transpose() const
        {
            matrix retn(this->c, this->r);
            for (unsigned int i = 0; i < this->c; i++)
                for (unsigned int j = 0; j < this->r; j++)
                    retn.m[i][j] = this->m[j][i];
            return retn;
        }

        double tr() const
        {
            if (this->r != this->c)
                throw SquareException();
            double result = 0;
            for (unsigned int i = 0; i < this->r; i++)
                result += this->m[i][i];
            return result;
        }

        double rank() const
        {
            matrix t = *this;//不改变自己
            unsigned int nonZero = t.r;//nonZero是矩阵最下面的非零行的下一行（第一个零行）
            for (unsigned int i = 0; i < nonZero; i++)
            {
                if (t.checkZero(i))
                {
                    nonZero--;
                    t.rswap(i, nonZero);
                }
            }
            for (unsigned int i = 0; i < nonZero; i++)
            {
                //现在第[i,nonZero)行全为非零行
                if (t.m[i][i] == 0.0) //防止除数为0
                {
                    for (unsigned int j = i + 1; j < t.c; j++)//第i行不为零行
                    {
                        if (t.m[i][j] != 0.0)
                        {
                            t.cswap(i, j);
                            break;
                        }
                    }
                }
                double x = t.m[i][i];
                for (unsigned int j = i + 1; j < nonZero; j++)
                    t.radd(j, i, t.m[j][i] / x);
                //更新nonZero，把所有零行放到矩阵下面
                for(unsigned int j = i; j < nonZero;j++)
                {
                    if (t.checkZero(j))
                    {
                        nonZero--;
                        t.rswap(j, nonZero);//将第j行和最后一个非零行互换
                    }
                }
            }
            return nonZero;
        }

        matrix Hadamard(const matrix& m) const
        {
            if (this->r != m.r || this->c != m.c)
                throw DimensionException();
            matrix retn(this->r,this->c);
            for (unsigned int i = 0; i < this->r; i++)
                for (unsigned int j = 0; j < this->c; j++)
                    retn.m[i][j] = this->m[i][j] * m.m[i][j];
            return retn;
        }

        matrix Kronecke(const matrix& m) const
        {
            unsigned int p = this->r * m.r, q = this->c * m.c;
            matrix retn(p, q);
            for (unsigned int i = 0; i < p; i++)
                for (unsigned int j = 0; j < q; j++)
                    retn.m[i][j] = this->m[i / m.r][j / this->c] * m.m[i % this->r][j % m.c];
            return retn;
        }

       void maxTri()
        {
            auto getUpZeroNum=[&](uint j)
            {
                uint num=0;
                for(uint i=0;i<this->r;i++)
                {
                    if(this->m[i][j]<Err)
                    {
                        this->m[i][j]=0;
                        num++;
                    }
                    else
                        break;
                }
                return num;
            };

            for(uint k=0;k<this->c;k++)
            {
                for(uint i=0;i<this->c-k;i++)
                {
                    if(getUpZeroNum(i)>getUpZeroNum(i+1))
                        this->cswap(i,i+1);
                }
            }
        }

        std::tuple<matrix,matrix,matrix> LUP() const;

        void radd(unsigned int r1, unsigned int r2, double m)
        {
            vector vr1=this->getRVector(r1);
            vector vr2=this->getRVector(r2);
            vector vr22=vr2.mul(m);
            this->setRVector(vr1.add(vr22),r1); //改变的是r1
        }

        void rmul(unsigned int r, double m)
        {
            vector vr=this->getRVector(r);
            this->setRVector(vr.mul(m),r);
        }

        void rswap(unsigned int r1, unsigned int r2)
        {
            vector vr1=this->getRVector(r1);
            vector vr2=this->getRVector(r2);
            this->setRVector(vr1,r2);
            this->setRVector(vr2,r1);
        }

        void cswap(unsigned int pos1, unsigned int  pos2)
        {
            double temp;
            for (unsigned int i = 0; i < this->r; i++)
            {
                temp = this->m[i][pos1];
                this->m[i][pos1] = this->m[i][pos2];
                this->m[i][pos2] = temp;
            }
        }

        void output() const
        {
            for (unsigned int i = 0; i < r; i++)
            {
                for (unsigned int j = 0; j < c; j++)
                {
                    printf("%g ", m[i][j]);
                }
                printf("\n");
            }
        }

        double* operator[](const uint i) const
        {
            return this->m[i];
        }

        static vector solve(matrix m, vector v) //克拉默法则求解
        {
                double D = m.det();
                vector result = vector(v.getl());
                for (unsigned int i = 0; i < v.getl(); i++) //逐行替换并计算行列式，目前替换第i列
                {
                        matrix mb = matrix(m);
                        for (unsigned int j = 0; j < v.getl(); j++) //替换第i列的第j个元素
                                mb.m[j][i] = v.v[j];
                        result.v[i] = mb.det() / D;
                }
                return result;
        }

        static matrix eye(unsigned int r, unsigned int c)
        {
            matrix retn(r, c);
            for (unsigned int  i = 0; i < (r < c ? r : c); i++)
                retn.m[i][i] = 1;
            return retn;
        }

        static matrix identity(unsigned int n)
        {
            return matrix::eye(n, n);
        }
};
