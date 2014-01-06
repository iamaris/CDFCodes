#ifndef ARIADNESETCOMMANDS_HH
#define ARIADNESETCOMMANDS_HH

#include "FrameUtil/APPCommand.hh"
#include <string>

class AppModule;

////////////////////////////////////////////////////////////////////////
// SetMstaCommand
////////////////////////////////////////////////////////////////////////
class SetMstaCommand : public APPCommand {

public:

  // Constructor
  SetMstaCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~SetMstaCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

////////////////////////////////////////////////////////////////////////
// SetParaCommand
////////////////////////////////////////////////////////////////////////
  class SetParaCommand : public APPCommand {

  public:

    // Constructor
    SetParaCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetParaCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };

////////////////////////////////////////////////////////////////////////
// SetPqmasCommand
////////////////////////////////////////////////////////////////////////
  class SetPqmasCommand : public APPCommand {

  public:

    // Constructor
    SetPqmasCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetPqmasCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
////////////////////////////////////////////////////////////////////////
// SetCaCommand
////////////////////////////////////////////////////////////////////////
  class SetCaCommand : public APPCommand {

  public:

    // Constructor
    SetCaCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetCaCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
////////////////////////////////////////////////////////////////////////
// SetPbCommand
////////////////////////////////////////////////////////////////////////
  class SetPbCommand : public APPCommand {

  public:

    // Constructor
    SetPbCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetPbCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
////////////////////////////////////////////////////////////////////////
// SetCfCommand
////////////////////////////////////////////////////////////////////////
  class SetCfCommand : public APPCommand {

  public:

    // Constructor
    SetCfCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetCfCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
////////////////////////////////////////////////////////////////////////
// SetXaCommand
////////////////////////////////////////////////////////////////////////
  class SetXaCommand : public APPCommand {

  public:

    // Constructor
    SetXaCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetXaCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
////////////////////////////////////////////////////////////////////////
// SetNbCommand
////////////////////////////////////////////////////////////////////////
  class SetNbCommand : public APPCommand {

  public:

    // Constructor
    SetNbCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetNbCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };

#endif
