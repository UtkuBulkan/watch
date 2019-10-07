#include "mysql_connection.h"
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <vector>

struct camera_list_item_t
{
	int id;
	std::string address;
	int is_record_as_output;
};

class DBConnection
{
public:
	DBConnection(const std::string& server, const std::string& user, const std::string& pass);
	~DBConnection();
	void add_camera(std::string camera_address);
	void get_camera_list(std::vector<camera_list_item_t> &camera_list);
	bool check_camera_exists(std::string camera_address);
	int sql();
	//static DBConnection* _default;

protected:
	void logLastError();
	std::string m_server; //eg: 192.168.1.1 or localhost:3306
	std::string m_user;
	std::string m_pass;
	std::string m_db;
	int m_port;
private:
	void preparetables();
	sql::mysql::MySQL_Driver *driver;
	sql::Connection *con;
	sql::Statement *statement;
	sql::PreparedStatement *prepare_statement;
};
//DBConnection* DBConnection::_default=NULL;
