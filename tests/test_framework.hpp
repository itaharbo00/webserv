/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_framework.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 17:12:53 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/01 17:19:41 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEST_FRAMEWORK_HPP
#define TEST_FRAMEWORK_HPP

#include <iostream>
#include <string>

// Macros pour les assertions
#define ASSERT_TRUE(cond, name) assertTrue((cond), (name))
#define ASSERT_EQUAL(exp, act, name) assertEqual((exp), (act), (name))

// Compteurs globaux pour les tests
static int	testsPassed = 0;
static int	testsFailed = 0;

// Fonctions d'assertion
inline void	assertTrue(bool condition, const std::string &testName)
{
	if (condition)
	{
		std::cout << "[OK]   " << testName << std::endl;
		testsPassed++;
	}
	else
	{
		std::cout << "[FAIL] " << testName << std::endl;
		testsFailed++;
	}
}

inline void	assertEqual(int expected, int actual, const std::string &testName)
{
	if (expected == actual)
	{
		std::cout << "[OK]   " << testName << std::endl;
		testsPassed++;
	}
	else
	{
		std::cout << "[FAIL] " << testName << " (Expected: " << expected
			<< ", Actual: " << actual << ")" << std::endl;
		testsFailed++;
	}
}

// Fonction pour afficher le résumé des tests
inline int	printTestSummary()
{
	std::cout << "\nTests passed: " << testsPassed << std::endl;
	std::cout << "Tests failed: " << testsFailed << std::endl;
	return testsFailed > 0; // Retourne 0 si tous les tests passent, sinon 1
}

#endif // TEST_FRAMEWORK_HPP
