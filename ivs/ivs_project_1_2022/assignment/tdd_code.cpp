//======== Copyright (c) 2021, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - priority queue code
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     Jiri Prokop <xproko47@stud.fit.vutbr.cz>
// $Date:       $2022-23-2
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author Jiri Prokop
 * 
 * @brief Implementace metod tridy prioritni fronty.
 */

#include <stdlib.h>
#include <stdio.h>

#include "tdd_code.h"

//============================================================================//
// ** ZDE DOPLNTE IMPLEMENTACI **
//
// Zde doplnte implementaci verejneho rozhrani prioritni fronty (Priority Queue)
// 1. Verejne rozhrani fronty specifikovane v: tdd_code.h (sekce "public:")
//    - Konstruktor (PriorityQueue()), Destruktor (~PriorityQueue())
//    - Metody Insert/Remove/Find/GetHead ...
//    - Pripadne vase metody definovane v tdd_code.h (sekce "protected:")
//
// Cilem je dosahnout plne funkcni implementace prioritni fronty implementovane
// pomoci tzv. "singly linked list", ktera bude splnovat dodane testy
// (tdd_tests.cpp).
//============================================================================//

PriorityQueue::PriorityQueue()
{
    m_pHead = NULL;
}

PriorityQueue::~PriorityQueue()
{
    struct Element_t* next = m_pHead, *current;    

    while(next != NULL)
    {
        current = next;
        next = next->pNext;
        delete(current);
    }
    m_pHead = NULL;
}

void PriorityQueue::Insert(int value)
{
    if (m_pHead == NULL)
    {
        struct Element_t* elem = new(struct Element_t);
        elem->pNext = NULL;
        elem->value = value;
        m_pHead = elem;
        return;
    }
    struct Element_t* current = m_pHead, *previous;
    while(current != NULL)
    {
        if (current->value <= value)
        {
            struct Element_t* new_el = new(struct Element_t);
            if (current == m_pHead)
            {
                new_el->pNext = current;
                m_pHead = new_el;

            }
            else
            {
                previous->pNext = new_el;
                new_el->pNext = current;
            }
            new_el->value = value;
            break;
        }
        previous = current;
        current = current->pNext;
    }
    if (current == NULL)
    {
        struct Element_t* new_el = new(struct Element_t);
        previous->pNext = new_el;
        new_el->pNext = NULL;
        new_el->value = value;
    }
}

bool PriorityQueue::Remove(int value)
{
    struct Element_t* current = m_pHead, *previous;
    while(current != NULL)
    {
        if (current->value == value)
        {
            struct Element_t* next = current->pNext;
            delete(current);
            if (current == m_pHead)
                m_pHead = next;
            else
                previous->pNext = next;
            return true;
        }
        previous = current;
        current = current->pNext;
    }
    return false;
}

PriorityQueue::Element_t *PriorityQueue::Find(int value)
{
    struct Element_t* current = m_pHead;
    while(current != NULL)
    {
        if (current->value == value)
        {
            return current;
        }
        current = current->pNext;
    }
    return NULL;
}

size_t PriorityQueue::Length()
{
    struct Element_t* current = m_pHead;
    size_t count = 0;
    while(current != NULL)
    {
        count++; 
        current = current->pNext;
    }
	return count;
}

PriorityQueue::Element_t *PriorityQueue::GetHead()
{
    return m_pHead;
}

/*** Konec souboru tdd_code.cpp ***/
