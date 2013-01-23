/*********************************************************************************
 *
 * This file is part of eSRO.
 *
 * eSRO is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * eSRO is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright ⓒ 2013  Rafael Dominguez (npcdoom)
 *
 *********************************************************************************/

#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cmath>
#include <cassert>

template <unsigned int N, typename T = double>
class Vector
{
public:

    Vector ()
    {
        for (unsigned int i = 0; i < N; ++i)
            m_data[i] = T();
    }

    ~Vector ()
    {
    }

    const T& operator () (unsigned int i) const
    {
        assert(i < N);

        return m_data[i];
    }

    T& operator () (unsigned int i)
    {
        assert(i < N);

        return m_data[i];
    }

    const T& operator [] (unsigned int i) const
    {
        assert(i < N);

        return m_data[i];
    }

    T& operator [] (unsigned int i)
    {
        assert(i < N);

        return m_data[i];
    }

    Vector<N,T> operator + (const Vector<N,T> &v) const
    {
        Vector<N,T> ret;

        for (unsigned int i = 0; i < N; ++i)
            ret.m_data[i] = m_data[i] + v.m_data[i];

        return ret;
    }

    Vector<N,T>& operator += (const Vector<N,T> &v)
    {
        for (unsigned int i = 0; i < N; ++i)
            m_data[i] += v.m_data[i];

        return *this;
    }

    Vector<N,T> operator - (const Vector<N,T> &v) const
    {
        Vector<N,T> ret;

        for (unsigned int i = 0; i < N; ++i)
            ret.m_data[i] = m_data[i] - v.m_data[i];

        return ret;
    }

    Vector<N,T>& operator -= (const Vector<N,T> &v)
    {
        for (unsigned int i = 0; i < N; ++i)
            m_data[i] -= v.m_data[i];

        return *this;
    }

    template <typename S>
    Vector<N,T> operator * (const S &scalar) const
    {
        Vector<N,T> ret;

        for (unsigned int i = 0; i < N; ++i)
            ret.m_data[i] = scalar*m_data[i];

        return ret;
    }

    template <typename S>
    Vector<N,T>& operator *= (const S &scalar)
    {
        for (unsigned int i = 0; i < N; ++i)
            m_data[i] *= scalar;

        return *this;
    }

    T squared_norm () const
    {
        T val = 0;

        for (unsigned int i = 0; i < N; ++i)
            val += (m_data[i]*m_data[i]);

        return val;
    }

    T norm () const
    {
        return sqrt(squared_norm());
    }

    void normalize ()
    {
        T n = norm();

        for (unsigned int i = 0; i < N; ++i)
            m_data[i] /= n;
    }

private:

    T m_data[N];
};


template <unsigned int N, typename T>
T dot_product (const Vector<N,T> &u, const Vector<N,T> &v)
{
    T ret = 0;

    for (unsigned int i = 0; i < N; ++i)
        ret += u(i)*v(i);

    return ret;
}

template <typename T>
Vector<3,T> cross_product (const Vector<3,T> &u, const Vector<3,T> &v)
{
    Vector<3,T> ret;

    ret(0) = u(1)*v(2)-u(2)*v(1);
    ret(1) = -(u(0)*v(2)-u(2)*v(0));
    ret(2) = u(0)*v(1)-u(1)*v(0);

    return ret;
}

#endif // VECTOR_HPP
