.data
stack: .space 32

.text
# $s0 for sp
# $s1 for bottom of the stack
la $s0, stack
addi $s0, $s0, 32
move $s1, $s0

# $s2 for n
li $v0, 5
syscall
move $s2, $v0

# $s3 for n//2, $s4 for n % 2
li $s3, 2
div $s2, $s3
mflo $s3
mfhi $s4

li $t1, 0
push:
slt $t0, $t1, $s3
beq $t0, $0, end_push
li $v0, 12
syscall
addi $s0, $s0, -1
sb $v0, 0($s0)
addi $t1, $t1, 1
j push
end_push:

beq $s4, $0, skip
li $v0, 12
syscall
skip:

li $t1, 0
pop:
slt $t0, $t1, $s3
beq $t0, $0, end_pop
li $v0, 12
syscall
lb $t2, 0($s0)
bne $v0, $t2, continue
addi $s0, $s0, 1
continue:
addi $t1, $t1, 1
j pop
end_pop:

li $v0, 1
beq $s0, $s1, yes
no:
li $a0, 0
j end
yes:
li $a0, 1
end:
syscall

li $v0, 10
syscall