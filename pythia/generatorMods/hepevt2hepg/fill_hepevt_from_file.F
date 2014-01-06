c     Fortran Functions that read the STDHEP file. Sebastian Carron (Duke)
c
      INTEGER FUNCTION HEPRED(istr,endoffile)
      IMPLICIT NONE
      INTEGER I,J
      include 'stdhep.inc' 
      include 'stdlun.inc'
      integer ifl,istr,lok,ntries,endoffile     
      call stdxrd(ifl,istr,lok)
      HEPRED = 0
      IF (ifl .EQ. 200) endoffile=1
      IF (lok .NE. 0 .AND. ifl .NE. 200) HEPRED=1
      RETURN
      END

      INTEGER FUNCTION HEPCLS(istr)
      IMPLICIT NONE
      integer istr
      include 'stdhep.inc'
C     close the stdhep file
      call stdxend(istr)        ! close input file
      HEPCLS=0  
      RETURN 
      END

      INTEGER FUNCTION HEPFIL(FNAME,istr,events,endoffile)
      IMPLICIT NONE 
      INTEGER ntries,events,endoffile,i
      CHARACTER*(*) FNAME
      include 'stdhep.inc' 
      include 'stdlun.inc'
      integer ilbl,istr,lok,ifl
      lnhwrt=0
      lnhrd=23
      lnhdcy=0
c      lnhout=22
      call mcfio_init()
      call stdxropen(FNAME,ntries,istr,lok)
      HEPFIL=0
      if (lok .NE. 0) then 
      HEPFIL=1
      RETURN
      end if
      do i=1,events
      call stdxrd(ifl,istr,lok)
      if (ifl .EQ. 200) then 
      endoffile=1
      HEPFIL=1
      RETURN
      end if
      if (lok .NE. 0) then 
      HEPFIL=1
      end if
      enddo      
      RETURN 
      END 
