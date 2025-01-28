/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:43:56 by bde-souz          #+#    #+#             */
/*   Updated: 2025/01/28 15:05:01 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

//isprintable para password
// talvez tamanho de 8

void checkIfValidPort(char **av)
{
	for (int i = 0; i < av[1]; i++)
	{
		/* code */
	}
	
}

void checkArguments(char **av)
{
	checkIfNumber(av);
}

int main(int ac, char **av)
{
	if (ac == 3)
	{
		std::cout << "NAO FODASE" << "\n";
		for (int i = 1; i < ac; i++)
		{
			std::cout << av[i] << "\n";
		}
		
	}
	else
	{
		std::cout << "FODASE" << "\n";
	}
}