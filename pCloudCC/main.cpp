#include <iostream>
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <iterator>
#include "pclsync_lib.h"
#include "control_tools.h"
namespace ct = control_tools;

static std::string version = "2.0.1";

int main(int argc, char **argv) {
  std::cout << "pCloud console client v."<< version << std::endl;
  std::string username;
  std::string password;
  bool daemon = false;
  bool commands = false;
  bool commands_only = false;
  bool newuser = false;
  bool passwordsw = false;
  bool save_pass = false;
  
  try {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("username,u", po::value<std::string>(&username), "pCloud account name")
        ("password,p", po::bool_switch(&passwordsw), "pCloud account password")
        ("crypto,c", po::value<std::string>(), "Crypto password")
        ("passascrypto,y", po::value<std::string>(), "Use user password as crypto password also.")
        ("daemonize,d", po::bool_switch(&daemon), "Daemonize the process.")
        ("commands ,o", po::bool_switch(&commands), "Parent stays alive and processes commands. ")
        ("mountpoint,m", po::value<std::string>(), "Mount point where drive to be mounted.")
        ("commands_only,k", po::bool_switch(&commands_only),"Daemon already started pass only commands")
        ("newuser,n", po::bool_switch(&newuser), "Switch if this is a new user to be registered.")
        ("savepassword,s", po::bool_switch(&save_pass), "Save password in database.")
    ;

    po::variables_map vm;        
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    if (commands_only) {
      ct::process_commands();
      exit(0);
    }
    
    for (int i = 1; i < argc;++i)
      memset(argv[i],0,strlen(argv[i]));
    if (daemon){
      strncpy(argv[0], "pCloudDriveDeamon", strlen(argv[0]));
    } else {
      strncpy(argv[0], "pCloudDrive", strlen(argv[0]));
    }
    
    if (vm.count("help")) {
      std::cout << desc << "\n";
      return 0;
    }
    
    if ((!vm.count("username"))) {
      std::cout << "Username option is required!!!"  << "\n";
      return 1;
    }
    console_client::clibrary::get_lib().username_ = username;
    
    if (passwordsw) {
      console_client::clibrary::get_lib().get_pass_from_console();
    }
    
    if ((!vm.count("crypto")) && (!vm.count("passascrypto")) ){
      console_client::clibrary::get_lib().setup_crypto_ = false;
    } else {
      console_client::clibrary::get_lib().setup_crypto_ = true;
      if (vm.count("crypto"))
        console_client::clibrary::get_lib().crypto_pass_ = vm["crypto"].as<std::string>();
      else 
        console_client::clibrary::get_lib().crypto_pass_ = password;
    }
    
    if (vm.count("mountpoint"))
        console_client::clibrary::get_lib().mount_ = vm["mountpoint"].as<std::string>();
    
    console_client::clibrary::get_lib().newuser_ = newuser;
    console_client::clibrary::get_lib().save_pass_ = save_pass;
    console_client::clibrary::get_lib().daemon_ = daemon;
  }
  catch(std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
  catch(...) {
    std::cerr << "Exception of unknown type!\n";
  }

  
    if (daemon)
      ct::daemonize(commands);
    else {
      if (commands)
        std::cout << "Option commnads/o  ignored."  << "\n";
      if (!console_client::clibrary::init())
        sleep(360000);
    }
  
  return 0;
}
