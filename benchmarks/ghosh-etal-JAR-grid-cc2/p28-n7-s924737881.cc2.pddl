(DEFINE (PROBLEM N7-S924737881) (:DOMAIN GRID)
 (:OBJECTS BORDER - CELL E4 - CELL E3 - CELL E2 - CELL E1 - CELL D4 - CELL D3 -
  CELL D2 - CELL C2 - CELL C1 - CELL RIGHT - SIDE LEFT - SIDE DOWN - SIDE UP -
  SIDE)
 (:INIT (DISPENSER C1) (PROC C2) (PROC D2) (PROC D3) (DISPENSER D4) (PROC E1)
  (PROC E2) (PROC E3) (PROC E4) (REJECT C1 LEFT) (REJECT C1 RIGHT)
  (REJECT C1 UP) (REJECT C1 DOWN) (AVAILABLE C2) (AVAILABLE D2) (AVAILABLE D3)
  (REJECT D4 LEFT) (REJECT D4 RIGHT) (REJECT D4 UP) (REJECT D4 DOWN)
  (AVAILABLE E1) (AVAILABLE E2) (AVAILABLE E3) (AVAILABLE E4)
  (REJECT BORDER LEFT) (REJECT BORDER RIGHT) (REJECT BORDER UP)
  (REJECT BORDER DOWN) (NEIGHBOUR LEFT BORDER C1) (NEIGHBOUR RIGHT C1 BORDER)
  (NEIGHBOUR RIGHT C2 C1) (NEIGHBOUR LEFT C1 C2) (NEIGHBOUR UP BORDER C1)
  (NEIGHBOUR DOWN C1 BORDER) (NEIGHBOUR DOWN BORDER C1)
  (NEIGHBOUR UP C1 BORDER) (NEIGHBOUR RIGHT BORDER C2)
  (NEIGHBOUR LEFT C2 BORDER) (NEIGHBOUR UP BORDER C2)
  (NEIGHBOUR DOWN C2 BORDER) (NEIGHBOUR DOWN D2 C2) (NEIGHBOUR UP C2 D2)
  (NEIGHBOUR LEFT BORDER D2) (NEIGHBOUR RIGHT D2 BORDER)
  (NEIGHBOUR RIGHT D3 D2) (NEIGHBOUR LEFT D2 D3) (NEIGHBOUR DOWN E2 D2)
  (NEIGHBOUR UP D2 E2) (NEIGHBOUR RIGHT D4 D3) (NEIGHBOUR LEFT D3 D4)
  (NEIGHBOUR UP BORDER D3) (NEIGHBOUR DOWN D3 BORDER) (NEIGHBOUR DOWN E3 D3)
  (NEIGHBOUR UP D3 E3) (NEIGHBOUR RIGHT BORDER D4) (NEIGHBOUR LEFT D4 BORDER)
  (NEIGHBOUR UP BORDER D4) (NEIGHBOUR DOWN D4 BORDER) (NEIGHBOUR DOWN E4 D4)
  (NEIGHBOUR UP D4 E4) (NEIGHBOUR LEFT BORDER E1) (NEIGHBOUR RIGHT E1 BORDER)
  (NEIGHBOUR RIGHT E2 E1) (NEIGHBOUR LEFT E1 E2) (NEIGHBOUR UP BORDER E1)
  (NEIGHBOUR DOWN E1 BORDER) (NEIGHBOUR DOWN BORDER E1)
  (NEIGHBOUR UP E1 BORDER) (NEIGHBOUR RIGHT E3 E2) (NEIGHBOUR LEFT E2 E3)
  (NEIGHBOUR DOWN BORDER E2) (NEIGHBOUR UP E2 BORDER) (NEIGHBOUR RIGHT E4 E3)
  (NEIGHBOUR LEFT E3 E4) (NEIGHBOUR DOWN BORDER E3) (NEIGHBOUR UP E3 BORDER)
  (NEIGHBOUR RIGHT BORDER E4) (NEIGHBOUR LEFT E4 BORDER)
  (NEIGHBOUR DOWN BORDER E4) (NEIGHBOUR UP E4 BORDER))
 (:GOAL
  (AND (BUSY C2)
       (BUSY D3)
       (BUSY E1)
       (BUSY E2)
       (BUSY E3)
       (FORALL (?C - CELL) (DISABLED-CONTROL-NEWJOB-GET-BUSY ?C))
       (FORALL (?P - CELL ?N1 - CELL ?S1 - SIDE ?S1OP - SIDE)
        (DISABLED-CONTROL-LOST-ACCEPTED-REQUEST ?P ?N1 ?S1 ?S1OP))
       (FORALL (?P - CELL ?N1 - CELL ?S1 - SIDE ?S1OP - SIDE)
        (DISABLED-CONTROL-LOST-REQUEST ?P ?N1 ?S1 ?S1OP))
       (FORALL (?P - CELL ?N1 - CELL ?S1 - SIDE ?S1OP - SIDE)
        (DISABLED-CONTROL-FINISH-RECD ?P ?N1 ?S1 ?S1OP))
       (FORALL (?P - CELL ?N1 - CELL ?S1 - SIDE ?S1OP - SIDE)
        (DISABLED-CONTROL-MARK-SENT ?P ?N1 ?S1 ?S1OP))
       (FORALL (?P - CELL ?N1 - CELL ?S1 - SIDE ?S1OP - SIDE)
        (DISABLED-CONTROL-RECV ?P ?N1 ?S1 ?S1OP))
       (FORALL (?P - CELL ?N1 - CELL ?S1 - SIDE ?S1OP - SIDE)
        (DISABLED-CONTROL-SEND ?P ?N1 ?S1 ?S1OP))
       (FORALL (?C - CELL ?N1 - CELL ?N2 - CELL ?N3 - CELL)
        (DISABLED-CONTROL-REJECT-PARENT-DOWN ?C ?N1 ?N2 ?N3))
       (FORALL (?C - CELL ?N1 - CELL ?N2 - CELL ?N3 - CELL)
        (DISABLED-CONTROL-REJECT-PARENT-UP ?C ?N1 ?N2 ?N3))
       (FORALL (?C - CELL ?N1 - CELL ?N2 - CELL ?N3 - CELL)
        (DISABLED-CONTROL-REJECT-PARENT-RIGHT ?C ?N1 ?N2 ?N3))
       (FORALL (?C - CELL ?N1 - CELL ?N2 - CELL ?N3 - CELL)
        (DISABLED-CONTROL-REJECT-PARENT-LEFT ?C ?N1 ?N2 ?N3))
       (FORALL (?C - CELL ?N2 - CELL ?S1 - SIDE ?S1OP - SIDE)
        (DISABLED-CONTROL-REJECT-NON-PARENT ?C ?N2 ?S1 ?S1OP))
       (FORALL (?C - CELL ?N2 - CELL ?S1 - SIDE ?S1OP - SIDE ?SPARENT - SIDE)
        (DISABLED-CONTROL-ACCEPT-PASSON ?C ?N2 ?S1 ?S1OP ?SPARENT))
       (FORALL (?P - CELL ?N1 - CELL ?S1 - SIDE ?S1OP - SIDE)
        (DISABLED-CONTROL-ACCEPT ?P ?N1 ?S1 ?S1OP))
       (FORALL (?P - CELL ?N1 - CELL)
        (DISABLED-CONTROL-REQUEST-PASSON-DOWN ?P ?N1))
       (FORALL (?P - CELL ?N1 - CELL)
        (DISABLED-CONTROL-REQUEST-PASSON-UP ?P ?N1))
       (FORALL (?P - CELL ?N1 - CELL)
        (DISABLED-CONTROL-REQUEST-PASSON-RIGHT ?P ?N1))
       (FORALL (?P - CELL ?N1 - CELL)
        (DISABLED-CONTROL-REQUEST-PASSON-LEFT ?P ?N1))
       (FORALL (?C - CELL ?N1 - CELL ?N2 - CELL ?N3 - CELL ?N4 - CELL)
        (DISABLED-CONTROL-MARK-AVAILABLE ?C ?N1 ?N2 ?N3 ?N4))
       (FORALL (?C - CELL)
        (DISABLED-CONTROL-UNBAR-NEIGHS-NOT-THRESHOLD-REACH ?C))
       (FORALL (?C - CELL) (DISABLED-CONTROL-UNBAR-NEIGHS-NOT-BUSY ?C))
       (FORALL (?C - CELL ?N - CELL ?S - SIDE)
        (DISABLED-CONTROL-MARK-UNAVAILABLE-NEIGHBOUR-THRESHOLD-REACHED ?C ?N
         ?S))
       (FORALL (?C - CELL) (DISABLED-CONTROL-BAR-NEIGHS-AVAIL ?C))
       (FORALL (?C - CELL)
        (DISABLED-CONTROL-MARK-UNAVAILABLE-THRESHOLD-EXCEED ?C))
       (FORALL (?C - CELL) (DISABLED-CONTROL-MARK-UNAVAILABLE-BUSY ?C))
       (FORALL (?C - CELL ?N1 - CELL ?S1 - SIDE ?N2 - CELL ?S2 - SIDE)
        (DISABLED-CONTROL-UNMARK-EXCEED ?C ?N1 ?S1 ?N2 ?S2))
       (FORALL
        (?C - CELL ?N1 - CELL ?S1 - SIDE ?N2 - CELL ?S2 - SIDE ?N3 - CELL ?S3 -
         SIDE)
        (DISABLED-CONTROL-MARK-THRESHOLD-EXCEED ?C ?N1 ?S1 ?N2 ?S2 ?N3 ?S3))
       (FORALL
        (?C - CELL ?N1 - CELL ?S1 - SIDE ?N2 - CELL ?S2 - SIDE ?N3 - CELL ?S3 -
         SIDE)
        (DISABLED-CONTROL-UNMARK-THRESHOLD-REACH ?C ?N1 ?S1 ?N2 ?S2 ?N3 ?S3))
       (FORALL (?C - CELL ?N1 - CELL ?S1 - SIDE ?N2 - CELL ?S2 - SIDE)
        (DISABLED-CONTROL-MARK-THRESHOLD-REACH ?C ?N1 ?S1 ?N2 ?S2)))))
