(define (problem BLOCKS-13-1)
(:domain BLOCKS)
(:objects I M G H L A C D E K F B J )
(:INIT (ONTABLE F) (ONTABLE K) (ON J E) (ON E D) (ON D C)
 (ON C A) (ON A L) (ON L H) (ON H G) (ON G M) (ON M I) (ON I F) (ON B K)
 )
(:goal (AND (ON D A) (ON A E) (ON E L) (ON L M) (ON M C) (ON C J) (ON J F)
            (ON F K) (ON K G) (ON G H) (ON H I) (ON I B)))
)