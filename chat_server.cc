#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <mutex>
#include <vector>
#include <sstream>
#include <nlohmann/json.hpp>

namespace asio = boost::asio;
using asio::ip::tcp;
using nlohmann::json;

class SocketWrapper {
		static int s_current_id;
		static std::mutex mutex;
	public:
		using socket_ptr = std::unique_ptr<tcp::socket>;
		using socket_weak_ptr = std::weak_ptr<tcp::socket>;
		SocketWrapper(tcp::socket&& socket) : m_socket(new tcp::socket(std::move(socket))),m_id(get_new_id()) {}
		tcp::socket& get_socket() { return *m_socket; }
		tcp::socket* get_socket_ptr() { return m_socket.get(); }
		int get_id() { return m_id; }
	private:
		static int get_new_id() {
			std::lock_guard guard(mutex);
			return s_current_id++;
		}

		socket_ptr m_socket;
		int m_id;
};

int SocketWrapper::s_current_id = 0;
std::mutex SocketWrapper::mutex;

class SocketHandler {
	public:
		void start_handling( tcp::socket&& socket ) {
			SocketWrapper wrapper(std::move(socket));
			std::thread handling_thread(&SocketHandler::handle,this,wrapper.get_socket_ptr(),wrapper.get_id());
			handling_thread.detach();
			this->add(std::move(wrapper));
		}
		void handle( tcp::socket* socket, int socket_id ) {
			try {
				while (true) {
					asio::streambuf buf(4096);
					auto buffer = buf.prepare(4096);
					boost::system::error_code ec;
					int len = socket->read_some(buffer,ec);
					if (ec == asio::error::eof || ec)
						break;
					buf.commit(len);
					std::ostringstream oss;
					oss << &buf;
					auto str = oss.str();
					auto message = json::parse(str);
					std::cout << message.dump() << std::endl;
					this->execute_json_command(message);
				}
			} catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
			}
			this->remove(socket_id);
		}

		void execute_json_command(const json& data) {
			std::lock_guard guard(mutex);
			try {
				if ( data["type"].get<std::string>() == "message" ) {
					auto user = data["name"].get<std::string>();
					auto user_message = data["msg"].get<std::string>();
					
					json message;
					message["type"] = "msg";
					message["name"] = user;
					message["msg"] = user_message;
					auto msg_str = message.dump();

					for ( auto& socket : sockets ) {
						asio::write(socket.get_socket(),asio::buffer(msg_str));
					}
				}
			} catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
			}
		}

		SocketWrapper& add( SocketWrapper&& wrapper ) {
			std::lock_guard guard(mutex);
			sockets.push_back(std::move(wrapper));
			return wrapper;
		}
		void remove( int id ) {
			std::lock_guard guard(mutex);
			std::erase_if( sockets, [&]( SocketWrapper& t_wrapper ) { return t_wrapper.get_id() == id; } );
		}
	private:
		std::mutex mutex;
		std::vector<SocketWrapper> sockets;
};

int main(int argc, char** argv) {
	try {
		asio::io_context io_context;
		tcp::acceptor acceptor(io_context, tcp::endpoint( tcp::v4(), 7778 ));
		SocketHandler handler;

		while ( true ) {
			auto socket = acceptor.accept();
			handler.start_handling(std::move(socket));
		}
	} catch ( std::exception& e ) {
		std::cerr << e.what() << std::endl;
	}
}
