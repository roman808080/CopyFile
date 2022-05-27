#include <array>
#include <iostream>
#include <memory>

#include <filesystem>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/asio/read.hpp>

using boost::asio::awaitable;
using boost::asio::buffer;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::ip::tcp;
using boost::asio::use_awaitable;

struct proxy_state
{
  proxy_state(tcp::socket client)
    : client(std::move(client))
  {
  }

  tcp::socket client;
};

using proxy_state_ptr = std::shared_ptr<proxy_state>;

awaitable<void> connect_to_server(boost::asio::io_context& ctx)
{
	try
	{
		boost::asio::steady_timer theTimer(ctx, boost::asio::chrono::seconds(1));
		co_await theTimer.async_wait(use_awaitable);

		tcp::socket to_server(ctx);
		auto target = *tcp::resolver(ctx).resolve("127.0.0.1", "5555");

        std::string path_to_file{"C:\\Users\\roman\\OneDrive\\Documents\\kivy\\main.py"};
        std::size_t path_size = path_to_file.size();

		co_await to_server.async_connect(target, use_awaitable);

        std::array<char, 1024> data{ 0 };

        memcpy(&data, &path_size, sizeof(path_size));
		co_await async_write(to_server, buffer(data, sizeof(path_size)), use_awaitable);
		co_await async_write(to_server, buffer(path_to_file, path_to_file.size()), use_awaitable);

        std::size_t file_size = std::filesystem::file_size(path_to_file);
        memcpy(&data, &file_size, sizeof(path_size));
		co_await async_write(to_server, buffer(data, sizeof(file_size)), use_awaitable);

        std::ifstream input_file(path_to_file, std::ios::in | std::ios::binary);
        std::size_t bytes_to_read = file_size;

        const std::size_t max_bytes_to_read {1024};

        do {
            std::size_t current_size {max_bytes_to_read};

            if (max_bytes_to_read >= bytes_to_read)
            {
                current_size = bytes_to_read;
                bytes_to_read = 0;
            }
            else
            {
                bytes_to_read -= max_bytes_to_read;
            }

			input_file.read(&data[0], current_size);
			co_await async_write(to_server, buffer(data, current_size), use_awaitable);

        } while (bytes_to_read > 0);

        input_file.close();
	}
	catch (std::exception &e)
	{
		std::printf("echo Exception: %s\n", e.what());
	}
}

awaitable<void> write_to_file(proxy_state_ptr state)
{
  try
  {
    std::array<char, 1024> data {0};
    std::size_t path_size = 0;

	co_await boost::asio::async_read(state->client, buffer(data, sizeof(path_size)), use_awaitable);
	memcpy(&path_size, &data, sizeof(path_size));

    std::cout << "The current path_size: " << path_size << std::endl;

    std::string path_to_file;
	co_await boost::asio::async_read(state->client, buffer(data, path_size), use_awaitable);
    std::copy(data.begin(), data.end(), std::back_inserter(path_to_file));

    std::cout << "The current path: " << path_to_file << std::endl;

    std::size_t file_size = 0;
	co_await boost::asio::async_read(state->client, buffer(data, sizeof(file_size)), use_awaitable);
	memcpy(&file_size, &data, sizeof(file_size));

    std::cout << "The file size: " << file_size << std::endl;

	std::ofstream output_file("C:\\Users\\roman\\OneDrive\\Documents\\local.txt", std::ios::out | std::ios::binary);
    std::size_t bytes_to_read = file_size;

    while (bytes_to_read > 0)
    {
        std::size_t bytes = co_await state->client.async_read_some(buffer(data), use_awaitable);
        bytes_to_read -= bytes;
        output_file.write(&data[0], bytes);
    }

    output_file.close();
  }
  catch (const std::exception& e)
  {
    state->client.close();
  }
}

awaitable<void> handle_client(tcp::socket client)
{
  auto state = std::make_shared<proxy_state>(std::move(client));
  co_await write_to_file(state);
}

awaitable<void> listen(tcp::acceptor& acceptor)
{
  for (;;)
  {
    auto client = co_await acceptor.async_accept(use_awaitable);

    auto ex = client.get_executor();
    co_spawn(ex, handle_client(std::move(client)), detached);
  }
}

int main(int argc, char* argv[])
{
  try
  {
    boost::asio::io_context ctx;

    auto listen_endpoint =
      *tcp::resolver(ctx).resolve(
          "0.0.0.0",
          "5555",
          tcp::resolver::passive
        );

    tcp::acceptor acceptor(ctx, listen_endpoint);
    co_spawn(ctx, listen(acceptor), detached);

    co_spawn(ctx, connect_to_server(ctx), detached);

    ctx.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
