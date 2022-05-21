//======== Copyright (c) 2021, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - Tests suite
//
// $NoKeywords: $ivs_project_1 $white_box_code.cpp
// $Author:     Jiri Prokop <xproko47@stud.fit.vutbr.cz>
// $Date:       $2021-06-03
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author Jiri Prokop
 * 
 * @brief Implementace testu prace s maticemi.
 */

#include <vector>
#include "gtest/gtest.h"
#include "white_box_code.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy operaci nad maticemi. Cilem testovani je:
// 1. Dosahnout maximalniho pokryti kodu (white_box_code.cpp) testy.
// 2. Overit spravne chovani operaci nad maticemi v zavislosti na rozmerech 
//    matic.
//============================================================================//

class MatrixTest : public ::testing::Test
{
protected:
    Matrix matx;
    Matrix matx1;

    void GetMatrix1x1()
    {
        matx = Matrix();
    }

    void GetMatrix(size_t rows, size_t cols, bool OtherMatx)
    {
        if (!OtherMatx)
        {
            matx = Matrix(rows, cols);
        }
        else
        {
            matx1 = Matrix(rows, cols);
        }
    }
    void GetMatrixWithValues(size_t rows, size_t cols, double values[], bool OtherMatx)
    {
        int counter = 0;
        if (!OtherMatx)
        {
            matx = Matrix(rows, cols);
        }
        else
        {
            matx1 = Matrix(rows, cols);
        }
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                if (!OtherMatx)
                {
                    matx.set(i, j, values[counter++]);
                }
                else
                {
                    matx1.set(i, j, values[counter++]);
                }
            }
        }
    }
};

TEST_F(MatrixTest, Constructors)
{
    GetMatrix1x1();

    EXPECT_EQ(matx.get(0, 0), 0);

    EXPECT_ANY_THROW(GetMatrix(0, 1, 0));
    EXPECT_ANY_THROW(GetMatrix(2, 0, 0));

    GetMatrix(4, 6, 0);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            EXPECT_EQ(matx.get(i, j), 0);
        }
    }
}
//chybi test destructoru, idk jestli je treba

TEST_F(MatrixTest, SetValue)
{
    GetMatrix(4, 5, 0);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            EXPECT_TRUE(matx.set(i, j, i * j));
            EXPECT_EQ(matx.get(i, j), i * j);
            if (i > 2)
            {
                EXPECT_FALSE(matx.set(i, i * (j + 3), 4));
            }
            else
            {
                EXPECT_FALSE(matx.set(i - 3, j, 2));
            }
        }
    }
    EXPECT_TRUE(matx.set(2, 2, -1));
    EXPECT_TRUE(matx.set(3, 4, -20.12));
    EXPECT_TRUE(matx.set(0, 0, 0.0000001));

}

TEST_F(MatrixTest, SetAllValues)
{
    std::vector<std::vector<double>> vectors = {{1, 2}, {0, 1}, {12, 4}, {0, 5}, {100, 3} };
    GetMatrix(2, 2, 0);
    EXPECT_FALSE(matx.set(vectors));

    GetMatrix(1, 3, 0);
    EXPECT_FALSE(matx.set(vectors));

    GetMatrix(5, 2, 0);
    EXPECT_TRUE(matx.set(vectors));
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            EXPECT_EQ(matx.get(i, j), vectors[i][j]);
        }
    }
}

TEST_F(MatrixTest, GetValue)
{
    double normalValues[20] = {12, 34, 1000, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 12, };
    GetMatrixWithValues(4, 5, normalValues, 0);
    int counter = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            EXPECT_EQ(normalValues[counter++], matx.get(i, j));
        }
    }

    double spicyValues[9] = {-12, 0.0000001, 0.00001, 0.002423472, -128, -42.4242, -96.9696}; 
    GetMatrixWithValues(3, 3, spicyValues, 0);
    counter = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            EXPECT_EQ(spicyValues[counter++], matx.get(i, j));
            EXPECT_ANY_THROW(matx.get(10 * (i + 1), j));
            EXPECT_ANY_THROW(matx.get(i, -j - 1));
        }
    }
    
}

TEST_F(MatrixTest, IsEqual)
{
    //exceptions
    GetMatrix(3, 2, 0);
    GetMatrix(1, 2, 1);
    EXPECT_ANY_THROW(matx == matx1);

    GetMatrix(3, 1, 1);
    EXPECT_ANY_THROW(matx == matx1);
    EXPECT_TRUE(matx == matx);

    //basic functionality
    double values1[] = {0, -12, 2.728281, 3.14, 35, 12, 1, 8};
    double values2[] = {0, -12, 2.728281, 3.14, 35, 12, 1, 9};
    GetMatrixWithValues(2, 4, values1, 0);
    GetMatrixWithValues(2, 4, values1, 1);
    EXPECT_TRUE(matx == matx1);
    GetMatrixWithValues(2, 4, values2, 1);
    EXPECT_FALSE(matx == matx1);
}

TEST_F(MatrixTest, Plus)
{
    //exceptions
    GetMatrix(3, 5, 0);
    GetMatrix(1, 2, 1);
    EXPECT_ANY_THROW(matx + matx1);

    GetMatrix(5, 5, 1);
    EXPECT_ANY_THROW(matx + matx1);

    //basic functionality
    std::vector<std::vector<double>> ResultVectors = {{1, 2}, {0, 1}, 
                                    {12, 4}, {0, 5}, {100, 3}, {-10, -1}};

    double values1[12] = {150, 0, -12, 13, -17, 6, 4, 3, 67, 54, 11, 12};
    double values2[12] = {-149, 2, 12, -12, 29, -2, -4, 2, 33, -51, -21, -13};
    GetMatrixWithValues(6, 2, values1, 0);
    GetMatrixWithValues(6, 2, values2, 1);
    Matrix FooResult = matx + matx1;
    int counter = 0;
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            EXPECT_EQ(FooResult.get(i, j), values1[counter] + values2[counter++]);
            EXPECT_EQ(FooResult.get(i, j), matx.get(i, j) + matx1.get(i, j));
            EXPECT_EQ(FooResult.get(i, j), ResultVectors[i][j]);
        }
    }
}

TEST_F(MatrixTest, MultiplyByMatrix)
{
    //exception
    GetMatrix(2, 5, 0);
    GetMatrix(2, 2, 1);
    Matrix FooResult;
    EXPECT_ANY_THROW(FooResult = matx * matx1);

    //basic functionality
    std::vector<std::vector<double>> ResultVectors = {{-16, 10, -180}, {96, 111, -90}, 
                        {69, 117, -105}, {87, 117, -39}, {72, 90, -36}, {176, 217, -126}};

    double values1[18] = {39, -12, -1, 0, 3, 12, 42, 5, 1, 16, 9, 5, 6, 6, 6, 13, 11, 17};
    double values2[9] = {1, 2, -3, 4, 5, 6, 7, 8, -9};
    GetMatrixWithValues(6, 3, values1, 0);
    GetMatrixWithValues(3, 3, values2, 1);
    FooResult = matx * matx1;
    
    int counter = 0;
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            EXPECT_EQ(FooResult.get(i, j), ResultVectors[i][j]);
        }
    }
}

TEST_F(MatrixTest, MultiplyByConst)
{
    int MulipliersCount = 5;
    int MultiplyBy[MulipliersCount] = {0, -1, 1, 6, -4};
    for (int i = 0; i < MulipliersCount; i++)
    {
        int rows = abs(MultiplyBy[i])  + 1;
        int cols = 3;
        GetMatrix(rows, cols, 0);
        for (int j = 0; j < rows; j++)
        {
            for (int k = 0; k < cols; k++)
            {
                matx.set(j, k, i + j + k);
            }
        }

        GetMatrix(rows, cols, 1);
        matx1 = matx * MultiplyBy[i];

        for (int j = 0; j < rows; j++)
        {
            for (int k = 0; k < cols; k++)
            {
                EXPECT_EQ(matx1.get(j, k), matx.get(j, k) * MultiplyBy[i]);
            }
        }
    }
}

TEST_F(MatrixTest, SolveEquation_Exceptions)
{
    GetMatrix(2, 2, 0);
    std::vector<double> b = {{1, 2, 3}}, results;
    EXPECT_ANY_THROW(results = matx.solveEquation(b));

    GetMatrix(2, 3, 0);
    EXPECT_ANY_THROW(results = matx.solveEquation(b));

    b = {1, 2};
    double values1[4] = {0, };
    GetMatrixWithValues(2, 2, values1, 0);
    EXPECT_ANY_THROW(results = matx.solveEquation(b));

    double values2[4] = {1, 2, 1, 2};
    GetMatrixWithValues(2, 2, values2, 0);
    EXPECT_ANY_THROW(results = matx.solveEquation(b));

    double values3[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    GetMatrixWithValues(3, 3, values3, 0);
    EXPECT_ANY_THROW(results = matx.solveEquation(b));

    GetMatrix1x1();
    b = {{0}};
    EXPECT_ANY_THROW(results = matx.solveEquation(b));

    double values4[16] = {2, 0, 2, 0, 1, 1, 0, 0, 2, 1, 1, 0, 0, 0, 0, 1};
    GetMatrixWithValues(4, 4, values4, 0);
    b = {1, 2, 3, 4};
    EXPECT_ANY_THROW(results = matx.solveEquation(b));
}

TEST_F(MatrixTest, SolveEquation_BasicFunctionality)
{
    double values1[9] = {1, 1, -1, 2, -1, 2, 1, -3, 2};
    std::vector<double> b = {{-1, 8, 3}}, results;
    GetMatrixWithValues(3, 3, values1, 0);
    results = matx.solveEquation(b);
    EXPECT_EQ(results[0], 1);
    EXPECT_EQ(results[1], 2);
    EXPECT_EQ(results[2], 4);

    double values2[4] = {2, 3, 1, -1};
    b = {9, 2};
    GetMatrixWithValues(2, 2, values2, 0);
    results = matx.solveEquation(b);
    EXPECT_EQ(results[0], 3);
    EXPECT_EQ(results[1], 1);

    b = {1};
    double value[] = {1};
    GetMatrixWithValues(1, 1, value, 0);
    results = matx.solveEquation(b);
    EXPECT_EQ(results[0], value[0]);

}

TEST_F(MatrixTest, Transpose)
{
    int row = 2;
    double values1[row * row] = {1, 5, 2, 6};
    GetMatrixWithValues(row, row, values1, 0);
    std::vector<std::vector<double>> ResultVectors = {{1, 2}, {5, 6}};
    matx1 = matx.transpose();

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < row; j++)
        {
            EXPECT_EQ(matx1.get(i, j), ResultVectors[i][j]);
        }
    }    

    row = 3;
    double values2[row * row] = {1, 4, 7, 2, 5, 8, 3, 6, 9};
    GetMatrixWithValues(row, row, values2, 0);
    ResultVectors = {{1, 2, 3,}, {4, 5, 6}, {7, 8, 9}};
    matx1 = matx.transpose();
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < row; j++)
        {
            EXPECT_EQ(matx1.get(i, j), ResultVectors[i][j]);
        }
    }

    row = 4;
    double values3[row * row] = {1, 5, 9, 4, 2, 6, 1, 5, 3, 7, 2, 6, 4, 8, 3, 7};
    GetMatrixWithValues(row, row, values3, 0);
    ResultVectors = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 1, 2, 3}, {4, 5, 6, 7}};
    matx1 = matx.transpose();
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < row; j++)
        {
            EXPECT_EQ(matx1.get(i, j), ResultVectors[i][j]);
        }
    }
}

TEST_F(MatrixTest, Inverse)
{
    //exceptions
    GetMatrix(3, 4, 0);
    EXPECT_ANY_THROW(matx1 = matx.inverse());

    int row = 2;
    double values0[row * row] = {1, 2, 1, 2};
    GetMatrixWithValues(row, row, values0, 0);
    EXPECT_ANY_THROW(matx1 = matx.inverse());

    //basic functionality
    row = 2;
    double values1[row * row] = {1, 1, 1, 2};
    std::vector<std::vector<double>> ResultVectors = {{2, -1}, {-1, 1}};
    GetMatrixWithValues(row, row, values1, 0);
    matx1 = matx.inverse();

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < row; j++)
        {
            EXPECT_EQ(matx1.get(i, j), ResultVectors[i][j]);
        }
    }

    row = 3;
    double values2[row * row] = {2, 5, 7, 6, 3, 4, 5, -2, -3};
    ResultVectors = {{1, -1, 1}, {-38, 41, -34}, {27, -29, 24}};
    GetMatrixWithValues(row, row, values2, 0);
    matx1 = matx.inverse();

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < row; j++)
        {
            EXPECT_EQ(matx1.get(i, j), ResultVectors[i][j]);
        }
    }
}

/*** Konec souboru white_box_tests.cpp ***/
