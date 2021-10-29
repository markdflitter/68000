.global __main
.global __stack
.global start
start:
			MOVE.B #4,0xE00001
			MOVEA.L #__stack,%SP
			MOVE.B #5,0xE00001

			JSR __main
			CMP.L #1,%D0
			BEQ start

			MOVE.B #0,0xE00001

			MOVE.L 4,%A0
			JMP (%A0)
