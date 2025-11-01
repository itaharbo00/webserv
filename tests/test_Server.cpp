/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_Server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: itaharbo <itaharbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 14:12:49 by itaharbo          #+#    #+#             */
/*   Updated: 2025/11/01 17:21:08 by itaharbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_framework.hpp"
#include "Server.hpp"
#include <iostream>

// Test basique pour vérifier que le framework de test fonctionne
int	main()
{
	ASSERT_TRUE(true, "Basic true assertion test");
	ASSERT_EQUAL(42, 42, "Basic equality assertion test");

	return printTestSummary();
}
