#include "BotFunctor.hpp"

BotFunctor::BotFunctor( BotFunctor::handler_types new_handler_type ) {

	this->handler_type = new_handler_type;

}

BotFunctor::~BotFunctor() {

}

void BotFunctor::operator() ( string nick, string channel, string msg, IRC *circ ) {

}

void BotFunctor::operator() ( string nick, string channel, IRC *circ ) {

}

void BotFunctor::operator() ( string recvd_line, IRC *circ ) {

}

int BotFunctor::get_handler_type() {

	return this->handler_type;

}

BotFunctor::handler_types operator|( BotFunctor::handler_types ht1, BotFunctor::handler_types ht2 ) {

	return static_cast<BotFunctor::handler_types>( static_cast<int>( ht1 ) | static_cast<int>( ht2 ) );

}
