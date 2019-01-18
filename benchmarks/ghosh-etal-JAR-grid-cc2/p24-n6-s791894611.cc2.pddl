(DEFINE (PROBLEM N6-S791894611) (:DOMAIN GRID)
 (:OBJECTS BORDER - CELL C2 - CELL C1 - CELL B2 - CELL B1 - CELL B0 - CELL A2 -
  CELL A1 - CELL RIGHT - SIDE LEFT - SIDE DOWN - SIDE UP - SIDE)
 (:INIT (PROC A1) (DISPENSER A2) (PROC B0) (PROC B1) (PROC B2) (PROC C1)
  (PROC C2) (AVAILABLE A1) (REJECT A2 LEFT) (REJECT A2 RIGHT) (REJECT A2 UP)
  (REJECT A2 DOWN) (AVAILABLE B0) (AVAILABLE B1) (AVAILABLE B2) (AVAILABLE C1)
  (AVAILABLE C2) (REJECT BORDER LEFT) (REJECT BORDER RIGHT) (REJECT BORDER UP)
  (REJECT BORDER DOWN) (NEIGHBOUR LEFT BORDER A1) (NEIGHBOUR RIGHT A1 BORDER)
  (NEIGHBOUR RIGHT A2 A1) (NEIGHBOUR LEFT A1 A2) (NEIGHBOUR UP BORDER A1)
  (NEIGHBOUR DOWN A1 BORDER) (NEIGHBOUR DOWN B1 A1) (NEIGHBOUR UP A1 B1)
  (NEIGHBOUR RIGHT BORDER A2) (NEIGHBOUR LEFT A2 BORDER)
  (NEIGHBOUR UP BORDER A2) (NEIGHBOUR DOWN A2 BORDER) (NEIGHBOUR DOWN B2 A2)
  (NEIGHBOUR UP A2 B2) (NEIGHBOUR LEFT BORDER B0) (NEIGHBOUR RIGHT B0 BORDER)
  (NEIGHBOUR RIGHT B1 B0) (NEIGHBOUR LEFT B0 B1) (NEIGHBOUR UP BORDER B0)
  (NEIGHBOUR DOWN B0 BORDER) (NEIGHBOUR DOWN BORDER B0)
  (NEIGHBOUR UP B0 BORDER) (NEIGHBOUR RIGHT B2 B1) (NEIGHBOUR LEFT B1 B2)
  (NEIGHBOUR DOWN C1 B1) (NEIGHBOUR UP B1 C1) (NEIGHBOUR RIGHT BORDER B2)
  (NEIGHBOUR LEFT B2 BORDER) (NEIGHBOUR DOWN C2 B2) (NEIGHBOUR UP B2 C2)
  (NEIGHBOUR LEFT BORDER C1) (NEIGHBOUR RIGHT C1 BORDER)
  (NEIGHBOUR RIGHT C2 C1) (NEIGHBOUR LEFT C1 C2) (NEIGHBOUR DOWN BORDER C1)
  (NEIGHBOUR UP C1 BORDER) (NEIGHBOUR RIGHT BORDER C2)
  (NEIGHBOUR LEFT C2 BORDER) (NEIGHBOUR DOWN BORDER C2)
  (NEIGHBOUR UP C2 BORDER))
 (:GOAL
  (AND (BUSY A1)
       (BUSY B0)
       (BUSY B2)
       (BUSY C1)
       (BUSY C2)
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
