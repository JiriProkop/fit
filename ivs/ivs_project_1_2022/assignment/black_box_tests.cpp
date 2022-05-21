//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     Jiri Prokop <xproko47@stud.fit.vutbr.cz>
// $Date:       $2022-01-03
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author Jiri Prokop
 * 
 * @brief Implementace testu binarniho stromu.
 */

#include <vector>

#include "gtest/gtest.h"

#include "red_black_tree.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy Red-Black Tree, testujte nasledujici:
// 1. Verejne rozhrani stromu
//    - InsertNode/DeleteNode a FindNode
//    - Chovani techto metod testuje pro prazdny i neprazdny strom.
// 2. Axiomy (tedy vzdy platne vlastnosti) Red-Black Tree:
//    - Vsechny listove uzly stromu jsou *VZDY* cerne.
//    - Kazdy cerveny uzel muze mit *POUZE* cerne potomky.
//    - Vsechny cesty od kazdeho listoveho uzlu ke koreni stromu obsahuji
//      *STEJNY* pocet cernych uzlu.
//============================================================================//

class NonEmptyTree : public ::testing::Test
{
protected:
    virtual void SetUp() 
    {
        int numbers[] = { 8, 50, 81, 33, 42, 69, 21, 13, 11, 91, 40, 2, 99};
        for(int i = 0; i < 13; ++i)
            tree.InsertNode(numbers[i]);
    }

    BinaryTree tree;
};


class EmptyTree : public ::testing::Test
{
protected:
    BinaryTree tree;
};

class TreeAxioms : public ::testing::Test
{
protected:
    virtual void SetUp() 
    {
        int numbers[] = {-50, -10, -1, 0, 1, 10, 20, 25, 30, 35, 50, 60, 70, 80, 90,};
        for(int i = 0; i < 15; ++i)
            tree.InsertNode(numbers[i]);
    }

    BinaryTree tree;
};

TEST_F(EmptyTree, InsertNode)
{
    EXPECT_TRUE(tree.GetRoot() == NULL);
    std::pair<bool, BinaryTree::Node_t *> InsertInfo = tree.InsertNode(11);
    ASSERT_TRUE(tree.GetRoot());
    EXPECT_TRUE(InsertInfo.first);
    EXPECT_TRUE(tree.GetRoot() == InsertInfo.second);
    EXPECT_EQ(tree.GetRoot()->key, 11);

    InsertInfo = tree.InsertNode(31);
    EXPECT_TRUE(InsertInfo.first);
    EXPECT_EQ(InsertInfo.second->key, 31);

    InsertInfo = tree.InsertNode(-1);
    EXPECT_TRUE(InsertInfo.first);
    EXPECT_EQ(InsertInfo.second->key, -1);

    InsertInfo = tree.InsertNode(0);
    EXPECT_TRUE(InsertInfo.first);
    EXPECT_EQ(InsertInfo.second->key, 0);
}

TEST_F(EmptyTree, DeleteNode)
{
    EXPECT_FALSE(tree.DeleteNode(11));
}

TEST_F(EmptyTree, FindNode)
{
    EXPECT_FALSE(tree.FindNode(31));
}

TEST_F(NonEmptyTree, InsertNode)
{
    ASSERT_FALSE(tree.GetRoot() == NULL);
    std::pair<bool, BinaryTree::Node_t *> InsertInfo1 = tree.InsertNode(11);
    EXPECT_FALSE(InsertInfo1.first);
    EXPECT_EQ(InsertInfo1.second->key, 11);

    std::pair<bool, BinaryTree::Node_t *> InsertInfo2 = tree.InsertNode(71);
    EXPECT_TRUE(InsertInfo2.first);
    EXPECT_EQ(InsertInfo2.second->key, 71);   
}

TEST_F(NonEmptyTree, DeleteNode_One)
{
    EXPECT_FALSE(tree.DeleteNode(100));
    EXPECT_TRUE(tree.DeleteNode(69));
}

TEST_F(NonEmptyTree, DeleteNode_AllFromMin)
{
    int AllNodes[] = {2,8,11,13,21,33,40,42,50,69,81,91,99};
    for(int i = 0; i < 13; i++)
    {
        EXPECT_TRUE(tree.DeleteNode(AllNodes[i]));
    }
    EXPECT_TRUE(tree.GetRoot() == NULL);
}

TEST_F(NonEmptyTree, DeleteNode_AllFromMax)
{
    int AllNodesWithout2[] = {99,91,81,69,50,42,40,33,21,13,11,8};
    for(int i = 0; i < 12; i++)
    {
        EXPECT_TRUE(tree.DeleteNode(AllNodesWithout2[i]));
    }
    EXPECT_EQ(tree.GetRoot()->key, 2);
    EXPECT_TRUE(tree.DeleteNode(2));
    EXPECT_TRUE(tree.GetRoot() == NULL);
}

TEST_F(NonEmptyTree, FindNode_SpecialValues)
{
    BinaryTree::Node_t* pNode;

    EXPECT_FALSE(tree.FindNode(0));
    tree.InsertNode(0);
    pNode= tree.FindNode(0);
    ASSERT_TRUE(pNode);
    EXPECT_EQ(pNode->key, 0);

    EXPECT_FALSE(tree.FindNode(-1));
    tree.InsertNode(-1);
    pNode = tree.FindNode(-1);
    ASSERT_TRUE(pNode);
    EXPECT_EQ(pNode->key, -1);
}

TEST_F(NonEmptyTree, FindNode_Even)
{
    int values[] = {2, 8, 40, 42, 50, 1, 3, 125, 85, 15, 23};
    for (int i = 0; i < 11; i++)
    {
        if (values[i] % 2 == 0)
        {
            BinaryTree::Node_t* pNode= tree.FindNode(values[i]);
            ASSERT_TRUE(pNode);
            EXPECT_EQ(pNode->key, values[i]);
        }
        else
        {
            EXPECT_TRUE(tree.FindNode(values[i]) == NULL);
        }
    }
}

TEST_F(NonEmptyTree, FindNode_Odd)
{
    int values[] = {11, 13, 21, 33, 69, 81, 91, 99, 100, 20, 6, 256, 128};
    for (int i = 0; i < 13; i++)
    {
        if (values[i] % 2 == 1)
        {
            BinaryTree::Node_t* pNode = tree.FindNode(values[i]);
            ASSERT_TRUE(pNode);
            EXPECT_EQ(pNode->key, values[i]);
        }
        else
        {
            EXPECT_TRUE(tree.FindNode(values[i]) == NULL);
        }
    }
}

// Axiom 1. Vsechny listove uzly (tedy uzly bez potomku) jsou “cerne”.
TEST_F(TreeAxioms, Axiom1)
{
    std::vector<BinaryTree::Node_t *> leafes;
    tree.GetLeafNodes(leafes);
    for (BinaryTree::Node_t *node : leafes)
    {
        EXPECT_EQ(node->color, BinaryTree::BLACK);
    }
}

// Axiom 2. Pokud je uzel “cerveny”, pak jsou jeho oba potomci “cerne”.
TEST_F(TreeAxioms, Axiom2)
{
    std::vector<BinaryTree::Node_t *> AllNodes;
    tree.GetAllNodes(AllNodes);
    int RedChildCount;
    for (BinaryTree::Node_t *node : AllNodes)
    {
        if (node->color == BinaryTree::RED)
        {
            EXPECT_TRUE(node->pLeft->color == node->pRight->color && node->pLeft->color == BinaryTree::BLACK);
        }
    }
}

// Axiom 3. Kazda cesta od kazdeho listoveho uzlu ke koreni obsahuje stejny pocet “cernych” uzlu. 
TEST_F(TreeAxioms, Axiom3)
{
    std::vector<BinaryTree::Node_t *> leafes;
    tree.GetLeafNodes(leafes);
    int LeafCount = 0;
    for (BinaryTree::Node_t *node : leafes)
    {
        LeafCount++;
    }
    int LeafToRootBlackCount[LeafCount];
    int counter = 0;
    int BlackNodeCount;
    BinaryTree::Node_t* Proot = tree.GetRoot();
    for (BinaryTree::Node_t *node : leafes)
    {
        BlackNodeCount = 0;
        while(node != Proot)
        {
            if (node->color == BinaryTree::BLACK)
            {
                BlackNodeCount++;
            }
            node = node->pParent;
        }
        LeafToRootBlackCount[counter++] = BlackNodeCount;
    }
    for (int i = 1; i < LeafCount; i++)
    {
        EXPECT_EQ(LeafToRootBlackCount[0], LeafToRootBlackCount[i]);
    }
}

/*** Konec souboru black_box_tests.cpp ***/
