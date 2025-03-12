/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bde-souz <bde-souz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 14:43:56 by bde-souz          #+#    #+#             */
/*   Updated: 2025/03/12 19:03:42 by bde-souz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

bool isNumber(std::string &str)
{
	std::string::iterator it = str.begin();

	while (it != str.end())
	{
		if (!std::isdigit(*it))
			return false;
		it++;
	}
	return true;
}

void checkIfValidPort(std::string arg)
{
	std::string port = arg;

	if (!isNumber(port))
		throw std::runtime_error("Port must be a number");

	if (atoi(arg.c_str()) > 65535 || atof(arg.c_str()) > INT_MAX)
		throw std::runtime_error("Port not available in system range");
	
}

void checkIfValidPassWord(const std::string &pass)
{
	if (pass.size() > 8 || pass.size() < 3)
		throw std::runtime_error ("Invalid Password (invalid size, min 3 or max 8)");

	for(std::string::const_iterator it = pass.begin(); 
		it != pass.end(); it++)
	{
		if (!std::isprint(*it) || *it == ' ')
			throw std::runtime_error ("Invalid Password (invalid character)");
	}
}

int main(int ac, char **av)
{
	signal(SIGPIPE, SIG_IGN);
	if (ac == 3)
	{
		Ircserv Server;
		try
		{
			checkIfValidPort(av[1]);
			checkIfValidPassWord(av[2]);

			Server.createServer(av[2], std::atoi(av[1]));
			Server.acceptClients();
		}
		catch(const std::exception& e)
		{
			std::cerr << red << e.what() << '\n' << reset;
		}
	}
	else
	{
		std::cout << red << "Wrong arguments!" << "\n" << reset;
		std::cout << "You need to use ./ircserv" << \
			green << " <port> <password>"<< reset << "\n";
	}
	return (0);
}

// ==104342== 
// ==104342== Process terminating with default action of signal 13 (SIGPIPE)
// ==104342==    at 0x4CCB8B0: send (send.c:28)
// ==104342==    by 0x40FBB4: Ircserv::broadcastMessageToChannelExceptSender(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, int) (helpFunctions.cpp:216)
// ==104342==    by 0x411512: Ircserv::disconnectClientFromEveryChannel(int) (helpFunctions.cpp:339)
// ==104342==    by 0x408431: Ircserv::acceptClients() (Ircserv.cpp:152)
// ==104342==    by 0x40558D: main (main.cpp:64)
// ==104342== 
// ==104342== HEAP SUMMARY:
// ==104342==     in use at exit: 82,811 bytes in 40 blocks
// ==104342==   total heap usage: 3,696 allocs, 3,656 frees, 501,235 bytes allocated
// ==104342== 
// ==104342== 4 bytes in 1 blocks are still reachable in loss record 1 of 15
// ==104342==    at 0x4849013: operator new(unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
// ==104342==    by 0x40CD0F: std::__new_allocator<int>::allocate(unsigned long, void const*) (new_allocator.h:137)
// ==104342==    by 0x40CCA0: __gnu_cxx::__alloc_traits<std::allocator<int>, int>::allocate(std::allocator<int>&, unsigned long) (alloc_traits.h:133)
// ==104342==    by 0x40CADF: std::_Vector_base<int, std::allocator<int> >::_M_allocate(unsigned long) (stl_vector.h:378)
// ==104342==    by 0x40C67B: std::vector<int, std::allocator<int> >::_M_realloc_insert(__gnu_cxx::__normal_iterator<int*, std::vector<int, std::allocator<int> > >, int const&) (vector.tcc:453)
// ==104342==    by 0x40A3BC: std::vector<int, std::allocator<int> >::push_back(int const&) (stl_vector.h:1287)
// ==104342==    by 0x408598: Ircserv::acceptClients() (Ircserv.cpp:156)
// ==104342==    by 0x40558D: main (main.cpp:64)
// ==104342== 
// ==104342== 15 bytes in 1 blocks are still reachable in loss record 2 of 15
// ==104342==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
// ==104342==    by 0x4C4C58E: strdup (strdup.c:42)
// ==104342==    by 0x4C7EC10: tzset_internal (tzset.c:402)
// ==104342==    by 0x4C7F5A6: __tz_convert (tzset.c:577)
// ==104342==    by 0x4076C3: Ircserv::createServer(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, unsigned int) (Ircserv.cpp:20)
// ==104342==    by 0x40555F: main (main.cpp:63)
// ==104342== 
// ==104342== 20 bytes in 1 blocks are still reachable in loss record 3 of 15
// ==104342==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
// ==104342==    by 0x4C7E917: __tzstring_len (tzset.c:98)
// ==104342==    by 0x4C7E917: __tzstring (tzset.c:121)
// ==104342==    by 0x4C80264: __tzfile_read (tzfile.c:408)
// ==104342==    by 0x4C7EC23: tzset_internal (tzset.c:405)
// ==104342==    by 0x4C7F5A6: __tz_convert (tzset.c:577)
// ==104342==    by 0x4076C3: Ircserv::createServer(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, unsigned int) (Ircserv.cpp:20)
// ==104342==    by 0x40555F: main (main.cpp:63)
// ==104342== 
// ==104342== 20 bytes in 1 blocks are still reachable in loss record 4 of 15
// ==104342==    at 0x4849013: operator new(unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
// ==104342==    by 0x49C0109: void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct<char*>(char*, char*, std::forward_iterator_tag) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
// ==104342==    by 0x418C90: Ircserv::channelsStruct::channelsStruct(Ircserv::channelsStruct const&) (Ircserv.hpp:71)
// ==104342==    by 0x419A90: std::__new_allocator<Ircserv::channelsStruct>::construct(Ircserv::channelsStruct*, Ircserv::channelsStruct const&) (new_allocator.h:187)
// ==104342==    by 0x419724: void __gnu_cxx::__alloc_traits<std::allocator<Ircserv::channelsStruct>, Ircserv::channelsStruct>::construct<Ircserv::channelsStruct>(std::allocator<Ircserv::channelsStruct>&, Ircserv::channelsStruct*, Ircserv::channelsStruct const&) (alloc_traits.h:145)
// ==104342==    by 0x4197C9: std::vector<Ircserv::channelsStruct, std::allocator<Ircserv::channelsStruct> >::_M_realloc_insert(__gnu_cxx::__normal_iterator<Ircserv::channelsStruct*, std::vector<Ircserv::channelsStruct, std::allocator<Ircserv::channelsStruct> > >, Ircserv::channelsStruct const&) (vector.tcc:462)
// ==104342==    by 0x418BBF: std::vector<Ircserv::channelsStruct, std::allocator<Ircserv::channelsStruct> >::push_back(Ircserv::channelsStruct const&) (stl_vector.h:1287)
// ==104342==    by 0x416321: Ircserv::createNewChannel(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&) (join.cpp:28)
// ==104342==    by 0x417A45: Ircserv::commandJoin(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&) (join.cpp:174)
// ==104342==    by 0x408FD6: Ircserv::bufferReader(int, char*) (Ircserv.cpp:234)
// ==104342==    by 0x4085E4: Ircserv::acceptClients() (Ircserv.cpp:160)
// ==104342==    by 0x40558D: main (main.cpp:64)
// ==104342== 
// ==104342== 20 bytes in 1 blocks are still reachable in loss record 5 of 15
// ==104342==    at 0x4849013: operator new(unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
// ==104342==    by 0x49C0109: void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct<char*>(char*, char*, std::forward_iterator_tag) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
// ==104342==    by 0x418C90: Ircserv::channelsStruct::channelsStruct(Ircserv::channelsStruct const&) (Ircserv.hpp:71)
// ==104342==    by 0x419FBC: void std::_Construct<Ircserv::channelsStruct, Ircserv::channelsStruct>(Ircserv::channelsStruct*, Ircserv::channelsStruct const&) (stl_construct.h:128)
// ==104342==    by 0x419F0B: Ircserv::channelsStruct* std::__do_uninit_copy<Ircserv::channelsStruct*, Ircserv::channelsStruct*>(Ircserv::channelsStruct*, Ircserv::channelsStruct*, Ircserv::channelsStruct*) (stl_uninitialized.h:120)
// ==104342==    by 0x419EB4: Ircserv::channelsStruct* std::__uninitialized_copy<false>::__uninit_copy<Ircserv::channelsStruct*, Ircserv::channelsStruct*>(Ircserv::channelsStruct*, Ircserv::channelsStruct*, Ircserv::channelsStruct*) (stl_uninitialized.h:137)
// ==104342==    by 0x419E7C: Ircserv::channelsStruct* std::uninitialized_copy<Ircserv::channelsStruct*, Ircserv::channelsStruct*>(Ircserv::channelsStruct*, Ircserv::channelsStruct*, Ircserv::channelsStruct*) (stl_uninitialized.h:184)
// ==104342==    by 0x419E48: Ircserv::channelsStruct* std::__uninitialized_copy_a<Ircserv::channelsStruct*, Ircserv::channelsStruct*, Ircserv::channelsStruct>(Ircserv::channelsStruct*, Ircserv::channelsStruct*, Ircserv::channelsStruct*, std::allocator<Ircserv::channelsStruct>&) (stl_uninitialized.h:372)
// ==104342==    by 0x419C5C: Ircserv::channelsStruct* std::__uninitialized_move_if_noexcept_a<Ircserv::channelsStruct*, Ircserv::channelsStruct*, std::allocator<Ircserv::channelsStruct> >(Ircserv::channelsStruct*, Ircserv::channelsStruct*, Ircserv::channelsStruct*, std::allocator<Ircserv::channelsStruct>&) (stl_uninitialized.h:396)
// ==104342==    by 0x41983C: std::vector<Ircserv::channelsStruct, std::allocator<Ircserv::channelsStruct> >::_M_realloc_insert(__gnu_cxx::__normal_iterator<Ircserv::channelsStruct*, std::vector<Ircserv::channelsStruct, std::allocator<Ircserv::channelsStruct> > >, Ircserv::channelsStruct const&) (vector.tcc:486)
// ==104342==    by 0x418BBF: std::vector<Ircserv::channelsStruct, std::allocator<Ircserv::channelsStruct> >::push_back(Ircserv::channelsStruct const&) (stl_vector.h:1287)
// ==104342==    by 0x416321: Ircserv::createNewChannel(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&) (join.cpp:28)
// ==104342== 
// ==104342== 77 bytes in 1 blocks are still reachable in loss record 6 of 15
// ==104342==    at 0x4849013: operator new(unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
// ==104342==    by 0x49BD0BD: std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_mutate(unsigned long, unsigned long, char const*, unsigned long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
// ==104342==    by 0x49BEB4F: std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_append(char const*, unsigned long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
// ==104342==    by 0x41203B: std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > std::operator+<char, std::char_traits<char>, std::allocator<char> >(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, char const*) (basic_string.h:3484)
// ==104342==    by 0x411468: Ircserv::disconnectClientFromEveryChannel(int) (helpFunctions.cpp:338)
// ==104342==    by 0x408431: Ircserv::acceptClients() (Ircserv.cpp:152)
// ==104342==    by 0x40558D: main (main.cpp:64)
// ==104342== 
// ==104342== 103 bytes in 5 blocks are still reachable in loss record 7 of 15
// ==104342==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
// ==104342==    by 0x4C7E8BD: __tzstring_len (tzset.c:98)
// ==104342==    by 0x4C7E8BD: __tzstring (tzset.c:121)
// ==104342==    by 0x4C80264: __tzfile_read (tzfile.c:408)
// ==104342==    by 0x4C7EC23: tzset_internal (tzset.c:405)
// ==104342==    by 0x4C7F5A6: __tz_convert (tzset.c:577)
// ==104342==    by 0x4076C3: Ircserv::createServer(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, unsigned int) (Ircserv.cpp:20)
// ==104342==    by 0x40555F: main (main.cpp:63)
// ==104342== 
// ==104342== 128 bytes in 1 blocks are still reachable in loss record 8 of 15
// ==104342==    at 0x4849013: operator new(unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
// ==104342==    by 0x40ADAF: std::__new_allocator<pollfd>::allocate(unsigned long, void const*) (new_allocator.h:137)
// ==104342==    by 0x40AD40: __gnu_cxx::__alloc_traits<std::allocator<pollfd>, pollfd>::allocate(std::allocator<pollfd>&, unsigned long) (alloc_traits.h:133)
// ==104342==    by 0x40AB1F: std::_Vector_base<pollfd, std::allocator<pollfd> >::_M_allocate(unsigned long) (stl_vector.h:378)
// ==104342==    by 0x40A6EB: std::vector<pollfd, std::allocator<pollfd> >::_M_realloc_insert(__gnu_cxx::__normal_iterator<pollfd*, std::vector<pollfd, std::allocator<pollfd> > >, pollfd const&) (vector.tcc:453)
// ==104342==    by 0x409E9C: std::vector<pollfd, std::allocator<pollfd> >::push_back(pollfd const&) (stl_vector.h:1287)
// ==104342==    by 0x4082D7: Ircserv::acceptClients() (Ircserv.cpp:135)
// ==104342==    by 0x40558D: main (main.cpp:64)
// ==104342== 
// ==104342== 400 bytes in 1 blocks are still reachable in loss record 9 of 15
// ==104342==    at 0x4849013: operator new(unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
// ==104342==    by 0x419E0F: std::__new_allocator<Ircserv::channelsStruct>::allocate(unsigned long, void const*) (new_allocator.h:137)
// ==104342==    by 0x419DA0: __gnu_cxx::__alloc_traits<std::allocator<Ircserv::channelsStruct>, Ircserv::channelsStruct>::allocate(std::allocator<Ircserv::channelsStruct>&, unsigned long) (alloc_traits.h:133)
// ==104342==    by 0x419C0F: std::_Vector_base<Ircserv::channelsStruct, std::allocator<Ircserv::channelsStruct> >::_M_allocate(unsigned long) (stl_vector.h:378)
// ==104342==    by 0x41979B: std::vector<Ircserv::channelsStruct, std::allocator<Ircserv::channelsStruct> >::_M_realloc_insert(__gnu_cxx::__normal_iterator<Ircserv::channelsStruct*, std::vector<Ircserv::channelsStruct, std::allocator<Ircserv::channelsStruct> > >, Ircserv::channelsStruct const&) (vector.tcc:453)
// ==104342==    by 0x418BBF: std::vector<Ircserv::channelsStruct, std::allocator<Ircserv::channelsStruct> >::push_back(Ircserv::channelsStruct const&) (stl_vector.h:1287)
// ==104342==    by 0x416321: Ircserv::createNewChannel(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&) (join.cpp:28)
// ==104342==    by 0x417A45: Ircserv::commandJoin(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&) (join.cpp:174)
// ==104342==    by 0x408FD6: Ircserv::bufferReader(int, char*) (Ircserv.cpp:234)
// ==104342==    by 0x4085E4: Ircserv::acceptClients() (Ircserv.cpp:160)
// ==104342==    by 0x40558D: main (main.cpp:64)
// ==104342== 
// ==104342== 570 bytes in 11 blocks are still reachable in loss record 10 of 15
// ==104342==    at 0x4849013: operator new(unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
// ==104342==    by 0x49BD0BD: std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_mutate(unsigned long, unsigned long, char const*, unsigned long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
// ==104342==    by 0x49BEB4F: std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_append(char const*, unsigned long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
// ==104342==    by 0x42CA05: Ircserv::checkIfBufferHasEnd(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&) (signalHandler.cpp:64)
// ==104342==    by 0x408BB0: Ircserv::bufferReader(int, char*) (Ircserv.cpp:208)
// ==104342==    by 0x4085E4: Ircserv::acceptClients() (Ircserv.cpp:160)
// ==104342==    by 0x40558D: main (main.cpp:64)
// ==104342== 
// ==104342== 1,024 bytes in 1 blocks are still reachable in loss record 11 of 15
// ==104342==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
// ==104342==    by 0x4C22BA3: _IO_file_doallocate (filedoalloc.c:101)
// ==104342==    by 0x4C31CDF: _IO_doallocbuf (genops.c:347)
// ==104342==    by 0x4C30F5F: _IO_file_overflow@@GLIBC_2.2.5 (fileops.c:744)
// ==104342==    by 0x4C2F6D4: _IO_new_file_xsputn (fileops.c:1243)
// ==104342==    by 0x4C2F6D4: _IO_file_xsputn@@GLIBC_2.2.5 (fileops.c:1196)
// ==104342==    by 0x4C23FD6: fwrite (iofwrite.c:39)
// ==104342==    by 0x49ADB64: std::basic_ostream<char, std::char_traits<char> >& std::__ostream_insert<char, std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*, long) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
// ==104342==    by 0x49ADEBA: std::basic_ostream<char, std::char_traits<char> >& std::operator<< <std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*) (in /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.30)
// ==104342==    by 0x407D46: Ircserv::createServer(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, unsigned int) (Ircserv.cpp:73)
// ==104342==    by 0x40555F: main (main.cpp:63)
// ==104342== 
// ==104342== 2,182 bytes in 1 blocks are still reachable in loss record 12 of 15
// ==104342==    at 0x4848899: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
// ==104342==    by 0x4C7FAC2: __tzfile_read (tzfile.c:275)
// ==104342==    by 0x4C7EC23: tzset_internal (tzset.c:405)
// ==104342==    by 0x4C7F5A6: __tz_convert (tzset.c:577)
// ==104342==    by 0x4076C3: Ircserv::createServer(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, unsigned int) (Ircserv.cpp:20)
// ==104342==    by 0x40555F: main (main.cpp:63)
