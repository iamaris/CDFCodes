#ifndef PYTHIASHOWCOMMANDS_HH
#define PYTHIASHOWCOMMANDS_HH

#include "FrameUtil/APPCommand.hh"

#include <string>

class AppModule;

////////////////////////////////////////////////////////////////////////
// ShowMstuCommand
////////////////////////////////////////////////////////////////////////
class ShowMstuCommand : public APPCommand {

public:

  // Constructor
  ShowMstuCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~ShowMstuCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

////////////////////////////////////////////////////////////////////////
// ShowParuCommand
////////////////////////////////////////////////////////////////////////
  class ShowParuCommand : public APPCommand {

  public:

    // Constructor
    ShowParuCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~ShowParuCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };

////////////////////////////////////////////////////////////////////////
// ShowMstjCommand
////////////////////////////////////////////////////////////////////////
class ShowMstjCommand : public APPCommand {

public:

  // Constructor
  ShowMstjCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~ShowMstjCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

////////////////////////////////////////////////////////////////////////
// ShowParjCommand
////////////////////////////////////////////////////////////////////////
  class ShowParjCommand : public APPCommand {

  public:

    // Constructor
    ShowParjCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~ShowParjCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
////////////////////////////////////////////////////////////////////////
// ShowMstpCommand
////////////////////////////////////////////////////////////////////////
class ShowMstpCommand : public APPCommand {

public:

  // Constructor
  ShowMstpCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~ShowMstpCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

////////////////////////////////////////////////////////////////////////
// ShowParpCommand
////////////////////////////////////////////////////////////////////////
  class ShowParpCommand : public APPCommand {

  public:

    // Constructor
    ShowParpCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~ShowParpCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };

////////////////////////////////////////////////////////////////////////
// ShowMsubCommand
////////////////////////////////////////////////////////////////////////
class ShowMsubCommand : public APPCommand {

public:

  // Constructor
  ShowMsubCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~ShowMsubCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

////////////////////////////////////////////////////////////////////////
// ShowCkinCommand
////////////////////////////////////////////////////////////////////////
  class ShowCkinCommand : public APPCommand {

  public:

    // Constructor
    ShowCkinCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~ShowCkinCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };

////////////////////////////////////////////////////////////////////////
// ShowKfinCommand
////////////////////////////////////////////////////////////////////////
  class ShowKfinCommand : public APPCommand {

  public:

    // Constructor
    ShowKfinCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~ShowKfinCommand();

    // Required methods
    virtual int handle(int argc, char* argv[] );
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };

////////////////////////////////////////////////////////////////////////
// ShowPmasCommand
////////////////////////////////////////////////////////////////////////
  class ShowPmasCommand : public APPCommand {

  public:

    // Constructor
    ShowPmasCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~ShowPmasCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
 
////////////////////////////////////////////////////////////////////////
// ShowMdcyCommand
////////////////////////////////////////////////////////////////////////
  class ShowMdcyCommand : public APPCommand {

  public:

    // Constructor
    ShowMdcyCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~ShowMdcyCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
 
////////////////////////////////////////////////////////////////////////
// ShowMdmeCommand
////////////////////////////////////////////////////////////////////////
  class ShowMdmeCommand : public APPCommand {

  public:

    // Constructor
    ShowMdmeCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~ShowMdmeCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
 
////////////////////////////////////////////////////////////////////////
// ShowBratCommand
////////////////////////////////////////////////////////////////////////
  class ShowBratCommand : public APPCommand {

  public:

    // Constructor
    ShowBratCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~ShowBratCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
 
////////////////////////////////////////////////////////////////////////
// ShowKfdpCommand
////////////////////////////////////////////////////////////////////////
  class ShowKfdpCommand : public APPCommand {

  public:

    // Constructor
    ShowKfdpCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~ShowKfdpCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };

 
////////////////////////////////////////////////////////////////////////
// ShowIsetCommand
////////////////////////////////////////////////////////////////////////
  class ShowIsetCommand : public APPCommand {

  public:

    // Constructor
    ShowIsetCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~ShowIsetCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
   
////////////////////////////////////////////////////////////////////////
// ShowKfprCommand
////////////////////////////////////////////////////////////////////////
  class ShowKfprCommand : public APPCommand {

  public:

    // Constructor
    ShowKfprCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~ShowKfprCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
 
////////////////////////////////////////////////////////////////////////
// ShowCoefCommand
////////////////////////////////////////////////////////////////////////
  class ShowCoefCommand : public APPCommand {

  public:

    // Constructor
    ShowCoefCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~ShowCoefCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
 
////////////////////////////////////////////////////////////////////////
// SuperSymmetric parameters from /PYMSSM/ and /PYSSMT/
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// ShowImssCommand
////////////////////////////////////////////////////////////////////////
class ShowImssCommand : public APPCommand {

public:

  // Constructor
  ShowImssCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~ShowImssCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

////////////////////////////////////////////////////////////////////////
// ShowRmssCommand
////////////////////////////////////////////////////////////////////////
class ShowRmssCommand : public APPCommand {

public:

  // Constructor
  ShowRmssCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~ShowRmssCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

////////////////////////////////////////////////////////////////////////
// ShowItcmCommand
////////////////////////////////////////////////////////////////////////
class ShowItcmCommand : public APPCommand {

public:

  // Constructor
  ShowItcmCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~ShowItcmCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

////////////////////////////////////////////////////////////////////////
// ShowRtcmCommand
////////////////////////////////////////////////////////////////////////
class ShowRtcmCommand : public APPCommand {

public:

  // Constructor
  ShowRtcmCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~ShowRtcmCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

#endif













