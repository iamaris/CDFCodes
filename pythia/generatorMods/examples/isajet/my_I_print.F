C
      Subroutine my_I_print
C
C     Description:
C
C     This routine can be used to check that the HEPG bank is
C     readable from YBOS using the Fortran interface to stdhep
C
C     Author:
C     M. Shapiro   Jan 20. 1998

      Implicit none
#include "stdlun.inc"
#include "inc/bcs.inc"

      Integer Status
      Integer Hepbtc, Heprt
      Integer Ind, Inddat,I, Blocat

      Status = Blocat(IW,'HEPG',-1,Ind,Inddat)
      Write(6,*) ' HEPG event ', IW(inddat+1), ' from YBOS'
      Write(6,*) 'Number entries=',IW(inddat+2), ' Index First displaced= ',
     &  IW(inddat+3), ' number displace=',IW(inddat+4)
      Write(6,*) (IW(Inddat+I),I=0,5)
C
C     Zero stdhep
C     ===========
      call stdzero
C
C     Move HEPG bank into stdhep
C     ==========================
      call stdhepbtc()
C
C     Write output of stdhep to logical unit 42
C     ==========================================
      lnhout = 42
      call heplst(2)

      Return
      End
