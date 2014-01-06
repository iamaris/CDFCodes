#ifndef PYTHIASETCOMMANDS_HH
#define PYTHIASETCOMMANDS_HH

#include "FrameUtil/APPCommand.hh"
#include <string>

class AppModule;

////////////////////////////////////////////////////////////////////////
// SetMstuCommand
////////////////////////////////////////////////////////////////////////
class SetMstuCommand : public APPCommand {

public:

  // Constructor
  SetMstuCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~SetMstuCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

////////////////////////////////////////////////////////////////////////
// SetParuCommand
////////////////////////////////////////////////////////////////////////
  class SetParuCommand : public APPCommand {

  public:

    // Constructor
    SetParuCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetParuCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };

////////////////////////////////////////////////////////////////////////
// SetMstjCommand
////////////////////////////////////////////////////////////////////////
class SetMstjCommand : public APPCommand {

public:

  // Constructor
  SetMstjCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~SetMstjCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

////////////////////////////////////////////////////////////////////////
// SetParjCommand
////////////////////////////////////////////////////////////////////////
  class SetParjCommand : public APPCommand {

  public:

    // Constructor
    SetParjCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetParjCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
////////////////////////////////////////////////////////////////////////
// SetMstpCommand
////////////////////////////////////////////////////////////////////////
class SetMstpCommand : public APPCommand {

public:

  // Constructor
  SetMstpCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~SetMstpCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

////////////////////////////////////////////////////////////////////////
// SetParpCommand
////////////////////////////////////////////////////////////////////////
  class SetParpCommand : public APPCommand {

  public:

    // Constructor
    SetParpCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetParpCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };

////////////////////////////////////////////////////////////////////////
// SetMsubCommand
////////////////////////////////////////////////////////////////////////
class SetMsubCommand : public APPCommand {

public:

  // Constructor
  SetMsubCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~SetMsubCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

////////////////////////////////////////////////////////////////////////
// SetCkinCommand
////////////////////////////////////////////////////////////////////////
  class SetCkinCommand : public APPCommand {

  public:

    // Constructor
    SetCkinCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetCkinCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };


////////////////////////////////////////////////////////////////////////
// SetKfinCommand
////////////////////////////////////////////////////////////////////////
  class SetKfinCommand : public APPCommand {

  public:

    // Constructor
    SetKfinCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetKfinCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };

////////////////////////////////////////////////////////////////////////
// SetPmasCommand
////////////////////////////////////////////////////////////////////////
  class SetPmasCommand : public APPCommand {

  public:

    // Constructor
    SetPmasCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetPmasCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
 
////////////////////////////////////////////////////////////////////////
// SetMdcyCommand
////////////////////////////////////////////////////////////////////////
  class SetMdcyCommand : public APPCommand {

  public:

    // Constructor
    SetMdcyCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetMdcyCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
 
////////////////////////////////////////////////////////////////////////
// SetMdmeCommand
////////////////////////////////////////////////////////////////////////
  class SetMdmeCommand : public APPCommand {

  public:

    // Constructor
    SetMdmeCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetMdmeCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
 
////////////////////////////////////////////////////////////////////////
// SetBratCommand
////////////////////////////////////////////////////////////////////////
  class SetBratCommand : public APPCommand {

  public:

    // Constructor
    SetBratCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetBratCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
 
////////////////////////////////////////////////////////////////////////
// SetKfdpCommand
////////////////////////////////////////////////////////////////////////
  class SetKfdpCommand : public APPCommand {

  public:

    // Constructor
    SetKfdpCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetKfdpCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
  
////////////////////////////////////////////////////////////////////////
// SetKfprCommand
////////////////////////////////////////////////////////////////////////
  class SetKfprCommand : public APPCommand {

  public:

    // Constructor
    SetKfprCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetKfprCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
  
////////////////////////////////////////////////////////////////////////
// SetIsetCommand
////////////////////////////////////////////////////////////////////////
  class SetIsetCommand : public APPCommand {

  public:

    // Constructor
    SetIsetCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetIsetCommand();

    // Required methods
    virtual int handle(int argc, char* argv[]);
    virtual void show() const;
    virtual bool isShowable() const;
    virtual std::string description() const;

  };
  
////////////////////////////////////////////////////////////////////////
// SetCoefCommand
////////////////////////////////////////////////////////////////////////
  class SetCoefCommand : public APPCommand {

  public:

    // Constructor
    SetCoefCommand(const char* const theCommand, AppModule* theTarget);
    // Destructor
    ~SetCoefCommand();

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
// SetImssCommand
////////////////////////////////////////////////////////////////////////
class SetImssCommand : public APPCommand {

public:

  // Constructor
  SetImssCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~SetImssCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

////////////////////////////////////////////////////////////////////////
// SetRmssCommand
////////////////////////////////////////////////////////////////////////
class SetRmssCommand : public APPCommand {

public:

  // Constructor
  SetRmssCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~SetRmssCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

////////////////////////////////////////////////////////////////////////
// TechniColour parameters from /PYTCSM/ 
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// SetItcmCommand
////////////////////////////////////////////////////////////////////////
class SetItcmCommand : public APPCommand {

public:

  // Constructor
  SetItcmCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~SetItcmCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

////////////////////////////////////////////////////////////////////////
// SetRtcmCommand
////////////////////////////////////////////////////////////////////////
class SetRtcmCommand : public APPCommand {

public:

  // Constructor
  SetRtcmCommand(const char* const theCommand, AppModule* theTarget);
  // Destructor
  ~SetRtcmCommand();

  // Required methods
  virtual int handle(int argc, char* argv[]);
  virtual void show() const;
  virtual bool isShowable() const;
  virtual std::string description() const;

};

#endif

