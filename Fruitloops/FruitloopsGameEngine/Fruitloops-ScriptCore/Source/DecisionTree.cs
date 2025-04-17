/******************************************************************************
/*!
\file  DecisionTree.cs
\Proj name  Shroomy Doomy
\author  Benjamin
\date    Feb 03, 2024
\brief  This file contains the implementation of a DecisionTree and its nodes,
       which are used to evaluate conditions and execute actions accordingly.
       
All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;

namespace FruitLoops
{
    /**
     * @brief Represents a decision tree that can be evaluated to perform actions.
     *
     * The DecisionTree is constructed using a root node and evaluated by passing
     * in the elapsed time (deltaTime). The evaluation traverses the tree until an
     * action node is reached and executed.
     */
    public class DecisionTree
    {
        private readonly DecisionTreeNode root;

        /**
         * @brief Constructs a new DecisionTree with the specified root node.
         *
         * @param rootNode The root node of the decision tree.
         */
        public DecisionTree(DecisionTreeNode rootNode)
        {
            root = rootNode;
        }

        /**
         * @brief Evaluates the decision tree.
         *
         * The evaluation process starts at the root node and traverses the tree based on
         * the conditions until an action is executed.
         *
         * @param deltaTime The elapsed time passed to action nodes during evaluation.
         */
        public void Evaluate(float deltaTime)
        {
            root.Evaluate(deltaTime);
        }
    }

    /**
     * @brief Represents a node in the decision tree.
     *
     * A DecisionTreeNode can either be a test node or an action node. Test nodes contain a condition
     * (Test) and two branches (TrueBranch and FalseBranch) to follow based on the test result.
     * Action nodes contain an action to execute.
     */
    public class DecisionTreeNode
    {
        /**
         * @brief A function that returns a boolean value indicating the result of the test.
         *
         * This is used to decide which branch of the tree to traverse.
         */
        public Func<bool> Test;

        /**
         * @brief An action to be executed if this node is an action node.
         *
         * The action accepts a float parameter (deltaTime) for time-based calculations.
         */
        public Action<float> Action;

        /**
         * @brief The branch to evaluate if the Test returns true.
         */
        public DecisionTreeNode TrueBranch;

        /**
         * @brief The branch to evaluate if the Test returns false.
         */
        public DecisionTreeNode FalseBranch;

        /**
         * @brief Constructs a test node for the decision tree.
         *
         * This constructor creates a node that evaluates a condition (Test) and,
         * based on its result, traverses either the TrueBranch or the FalseBranch.
         *
         * @param test The function used to evaluate the condition.
         * @param trueBranch The node to evaluate if the test returns true.
         * @param falseBranch The node to evaluate if the test returns false.
         */
        public DecisionTreeNode(Func<bool> test, DecisionTreeNode trueBranch, DecisionTreeNode falseBranch)
        {
            Test = test;
            TrueBranch = trueBranch;
            FalseBranch = falseBranch;
        }

        /**
         * @brief Constructs an action node for the decision tree.
         *
         * This constructor creates a node that performs a specific action when evaluated.
         *
         * @param action The action to execute, which takes deltaTime as a parameter.
         */
        public DecisionTreeNode(Action<float> action)
        {
            Action = action;
        }

        /**
         * @brief Evaluates the decision tree node.
         *
         * If this node is an action node, its action is executed using the provided deltaTime.
         * If it is a test node, the Test function is invoked to decide which branch (TrueBranch
         * or FalseBranch) to evaluate next.
         *
         * @param deltaTime The elapsed time passed to action nodes during evaluation.
         */
        public void Evaluate(float deltaTime)
        {
            if (Action != null)
            {
                Action(deltaTime);
            }
            else if (Test.Invoke())
            {
                TrueBranch.Evaluate(deltaTime);
            }
            else
            {
                FalseBranch.Evaluate(deltaTime);
            }
        }
    }
}