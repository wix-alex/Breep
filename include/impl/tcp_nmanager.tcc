
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                               //
// Copyright 2017 Lucas Lazare.                                                                  //
// This file is part of Breep project which is released under the                                //
// European Union Public License v1.1. If a copy of the EUPL was                                 //
// not distributed with this software, you can obtain one at :                                   //
// https://joinup.ec.europa.eu/community/eupl/og_page/european-union-public-licence-eupl-v11     //
//                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////


#include "tcp_nmanager.hpp" // TODO: remove [Seems useless, but allows my IDE to work]

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp> // boost::uuids::to_string
#include <boost/array.hpp>
#include <boost/uuid/string_generator.hpp>
#include <memory>
#include <string>

#include "detail/utils.hpp"
#include "network.hpp"

template <typename data_container>
void breep::tcp_nmanager::send(commands command, const data_container& data, const peer<tcp_nmanager>& address) const {
	//todo
}

template <typename data_iterator>
void breep::tcp_nmanager::send(commands command, data_iterator begin, data_iterator end, const peer<tcp_nmanager>& address) const {
	//todo
}

breep::peer<breep::tcp_nmanager> breep::tcp_nmanager::connect(const boost::asio::ip::address& address, unsigned short port) {
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::iterator endpoint_iterator =
			resolver.resolve(boost::asio::ip::tcp::resolver::query(address.to_string(),std::to_string(port)));

	std::shared_ptr<boost::asio::ip::tcp::socket> socket = std::make_shared<boost::asio::ip::tcp::socket>(io_service);
	boost::asio::connect(*socket, endpoint_iterator);
	boost::asio::write(
			*socket,
			boost::asio::buffer(breep::detail::to_bigendian1(boost::uuids::to_string(m_owner->self().id())))
	);
	boost::system::error_code error;
	boost::array<char, 512> buffer;
	size_t len = socket->read_some(boost::asio::buffer(buffer), error);

	if (error) {
		return constant::bad_peer<tcp_nmanager>;
	}

	std::vector<char> input{};
	input.reserve(len);
	std::copy(buffer.cbegin(), buffer.cend(), std::back_inserter(input));
	return peer<tcp_nmanager>(
			boost::uuids::string_generator{}(breep::detail::to_bigendian2<std::string>(input)),
			boost::asio::ip::address(address),
	        std::move(socket)
	);
}

void breep::tcp_nmanager::disconnect(const peer<tcp_nmanager>&) {
	//todo
}

void breep::tcp_nmanager::owner(network<tcp_nmanager>* owner) {
	if (m_owner == nullptr) {
		m_owner = owner;
	} else {
		throw invalid_state("Tried to set an already set owner. This object shouldn't be shared.");
	}
}