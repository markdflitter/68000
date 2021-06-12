.global __main
.global start
start:
			MOVEA.L #0x2FFFFC,%SP
			JSR __main
			MOVE.L 4,%A0
			JMP (%A0)
