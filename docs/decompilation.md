# Decompilation guide

## Table of contents

- [Decompilation guide](#decompilation-guide)
  - [Table of contents](#table-of-contents)
  - [Setup](#setup)
  - [Decompilation process](#decompilation-process)
  - [Making a decomp.me scratch](#making-a-decompme-scratch)

## Setup

If you ran `setup.py`, you should have a dump of the debug symbols inside of the `dump/sym` folder. It includes everything you need regarding debug information (prototypes, local variables, globals, etc).

Look for any function that catches your eye, and search for it on the dumped debug symbols (`p3_functions.cpp`).

Paste it into the corresponding file, and start decompiling.

## Decompilation process

Let's imagine that you chose the function `exh_mbar_time_out` from `main/scrctrl.c`.

Let's begin by first taking a look at the info we have for this function:
  - Debug info for the function:
    - Name: `exh_mbar_time_out`
    - Signature: `static int exh_mbar_time_out(EXAM_CHECK*)`
    - Arguments:
      - `$a0` -> `EXAM_CHECK *ec_pp`
    - Local variables:
      - `$a1` -> `int ret`
  - Disassembly (of course):
    ```mipsasm
        lw    $v1, 0x32c($a0)
        lw    $v0, 0x28($a0)
        beqz  $v0, l_2c
        negu  $a1, $v1
        bnezl $v1, l_20
        lbu   $v1, 0x2c($a0)
        b     l_2c
        move  $a1, $zero
    l_20:
        lbu   $v0, 0x330($a0)
        xor   $v0, $v0, $v1
        movz  $a1, $zero, $v0
    l_2c:
        jr    $ra
        move  $v0, $a1
    ```

Given the above, you can start in two ways:
  1. Let a decompiler give you a head start (Ghidra, m2c):
      - Pros:
        * a.) Works great with small functions.
        * b.) Ghidra can figure GCC optimizations nicely.
        * c.) m2c is designed for matching decomp in mind.
      - Cons:
        * a.) Not efficient for big functions.
        * b.) m2c isn't very good at figuring GCC optimizations.
        * c.) Ghidra isn't built for matching decomp--its pseudocode won't match most of the times.
          * c.1) m2c isn't perfect for PS2, will sometimes fail.
      - I strongly recommend using both decompilers due to its strong and weak points.
        There are more decompiling tools around but these are the most recommended (for PS1/PS2).
  2. Translate assembly by hand:
      - Pros:
        * a.) Very efficient regardless of the function. 
        * b.) You will have a better view of the function.
      - Cons:
        * a.) Very time-consuming depending on the length and complexity of the function.
        * b.) Requires previous knowledge of how the compiler works and the optimizations it often applies.

I strongly recommend option 1 for starting and later going for option 2.<br>
Here I'll guide through option 2 as decompilers do most* of the work for you, so
let's take another look at the disassembly and slowly translate it to a C-like pseudocode.

But before that, let's take a moment to define what our pseudocode will be like:
  1. Every GPR will be a local variable.
  2. We will explicitly add type casts on loads and saves.
  3. For loads and saves, we will always assume that the base is a struct pointer unless suspected to be a pointer to a base C type.
      * Most of the times you won't have to do any assumptions thanks to the debug information (unless you're working with `prlib`).
  4. Struct fields will be prefixed with `unk` followed by the offset in hexadecimal and uppercase (e.g.: `unk1FF`).
  5. The function's real signature will be defined after the translation in the case that there is no debug information.
  6. Labels will be prefixed by `l_` followed by the function relative offset (e.g.: `l_4c`).

An example function would look like this after translation:
```c
void my_func(void) {
    a1 = (s32)a0->unk0;
    v0 = a1 + 1;
    return;
}
```

That out of the way, we can begin. We will explain an instruction once after first encountering it,
next instances of said instruction will be skipped to be brief.
  - First 'unknown' instruction is `lw`:
    - Defined as the following:
      - `Load Word: To load a word from memory as a signed value.`
      - `lw rt, offset(base)`
      - `GPR[rt] = memory[GPR[base] + offset];`
    - On our pseudocode, it would translate to the following:
      - `v1 = *(s32*)(a0 + 0x32c);`
    - Assuming `$a0` is a struct pointer (we will always assume so from now on), it would translate to the following:
      - `v1 = (s32)a0->unk32C;`
  - Second unknown instruction is `beqz`:
    - Branch instructions are a bit confusing, so hold on tight.
    - This is a pseudoinstruction, equivalent to `beq rs, $zero, offset`. The `beq` instruction will be explained instead.
    - MIPS has what's called a [delay slot](https://en.wikipedia.org/wiki/Delay_slot), it is basically an instruction slot that gets executed after a branch or jump instruction, but *before* the branch takes effect.<br>
      Given this example:
        ```mipsasm
        beq $a0, $a1, label
        nop
        ```
      You would think the `nop` instruction wouldn't be executed if the branch is taken--but it does.<br><br>
      This `nop` is executed regardless of whether the branch is taken or not. This slot (the `nop` in this case) is called the delay slot.<br><br>
      There are other types of delay slots (e.g. load delay slots), though the PS2's EE core only has branch delay slots.
        - GCC likes to sometimes reuse registers in branches, where you may see code like
          the following:
          ```mipsasm
          lw   $v0, 0x4($a1)
          beqz $v0, label
          lw   $v0, 0x4($a2)
          ```
          When the branch instruction is executed, it first makes the check (`$v0 == 0`) before
          executing the delay slot. Therefore it would look like this on our pseudocode:
          ```c
          v0 = (s32)a1->unk4;
          tmp = (v0 == 0);
          v0 = (s32)a2->unk4;
          if (tmp) {
              goto label;
          }
          ```
          Or to make things simpler and avoid making a temporary variable:
          ```c
          v0 = (s32)a1->unk4;
          if (v0 == 0) {
              v0 = (s32)a2->unk4;
              goto label;
          }
          v0 = (s32)a2->unk4;
          ```
          It's easy to be confused when looking at this register reuse (I was at the beginning), but once you get the hold of it, it'll be a breeze.<br>
        - Pseudocode equivalent:
          ```c
          [delay slot here if applicable]
          if (v0 == 0) {
              [delay slot here if applicable]
              goto l_2c;
          }
          ```
  - Third unknown instruction is `negu`:
    - Pseudoinstruction, equivalent to `subu rd, $zero, rt` (`GPR[rd] = 0 - GPR[rt]`).
    - Pseudocode equivalent:
      - `a1 = -v1;`
  - 4th unknown instruction is `bnezl`:
    - Pseudoinstruction, equivalent to `bnel rs, $zero, offset`. The `bnel` instruction will be explained instead.
    - MIPS has special 'likely' branches, which means:
      * a.) If the branch is taken, the delay slot is executed.
      * b.) If the branch is *not* taken, the delay slot is *not* executed.
      - Example:
        ```mipsasm
        bnel $v1, $zero, l_20
        lbu  $v1, 0x2c($a0)
        ```
        - If `$v1` is not zero, the word at `0x2c($a0)` is loaded to `$v1` (delay slot executed) and the branch is taken.
        - If `$v1` is zero, the branch isn't taken and the delay slot is skipped, leading to the next instruction.
      - Pseudocode equivalent:
        ```c
        if (v1 != 0) {
            v1 = (u8)a0->unk2C;
            goto l_20;
        }
        ```
  - 5th unknown instruction is `lbu`:
    - `Load Byte Unsigned: To load a byte from memory as an unsigned value.`
    - Pseudocode equivalent:
      - `$v1 = (u8)a0->unk2C;`
  - 6th unknown instruction is `b`:
    - Pseudoinstruction, equivalent to `beq $zero, $zero, offset` (branch always taken).
    - Pseudocode equivalent:
      - `goto l_offset;`
      - Don't forget the delay slot!
  - 7th unknown instruction is `move`:
    - Pseudoinstruction, equivalent to `daddu rd, rs, $zero`.
    - Pseudocode equivalent:
      - `rd = rs;` / `$a1 = 0;` (for this case)
  - 8th unknown instruction is `xor`:
    - `Exclusive OR: To calculate a bitwise logical EXCLUSIVE OR.`
    - Format: `xor rd, rs, rt`
    - Pseudocode equivalent: `v0 = v0 ^ v1;`
  - 9th unknown instruction is `movz`:
    - `Move Conditional on Zero: To move data between GPRs according to the value of a GPR.`
    - Format: `movz rd, rs, rt`
    - Pseudocode equivalent:
      ```c
      if (v0 == 0) {
          a1 = 0;
      }
      ```
  - 10th unknown instruction is `jr`:
    - `Jump Register: To branch to the address specified by a GPR.`
    - Format: `jr rs`
    - Pseudocode equivalent:
      - `goto rs;`
      - If `rs` is `$ra` (return address register): `return;`

For more instructions, check the ISA manual.

Having translated everything, the function should look like this:
```c
void exh_mbar_time_out(void) {
    v1 = (s32)a0->unk32C;
    v0 = (s32)a0->unk28;
    a1 = -v1;
    if (v0 == 0) {
        goto l_2c;
    }
    if (v1 != 0) {
        v1 = (u8)a0->unk2C;
        goto l_20;
    }
    a1 = 0;
    goto l_2c;
l_20:
    v0 = (u8)a0->unk330;
    v0 = v0 ^ v1;
    if (v0 == 0) {
        a1 = 0;
    }
l_2c:
    v0 = a1;
    return;
}
```

We've got a lot of nasty gotos and labels. Our goal will now be to get rid of those, going branch for branch, sweeping out each goto.

Let's look at the first branch:
  - `if (v0 == 0) goto l_2c;`
    - On this branch, if `v0` is zero, it makes the jump. If the condition fails though, no jump is made, and the rest of the code is executed up. A common thing is that the control flow eventually leads to `l_2c` regardless of the branches that are taken.<br><br>
    So, to remove the first goto, we can invert the condition, and insert all of the code into the if statement.<br>
    The function should look like this now:
      ```c
      void exh_mbar_time_out(void) {
          v1 = (s32)a0->unk32C;
          v0 = (s32)a0->unk28;
          a1 = -v1;
          if (v0 != 0) {
              if (v1 != 0) {
                  v1 = (u8)a0->unk2C;
                  goto l_20;
              }
              a1 = 0;
              goto l_2c;
      l_20:
              v0 = (u8)a0->unk330;
              v0 = v0 ^ v1;
              if (v0 == 0) {
                  a1 = 0;
              }
          }
      l_2c:
          v0 = a1;
          return;
      }
      ```
      And just like that, the goto is gone. Though, we cannot remove the label yet, as there is another branch pointing to that label. We'll take care of that shortly.
  - `if (v1 != 0) { [delay slot]; goto l_20; }`
    - Here we have a likely branch. The delay slot inside the if statement is irrelevant to us right now, our target is the goto--let's ignore it exists for a moment. Much better.<br><br>
    We can do the same thing we did earlier, and we will have gotten rid of the goto and `l_20` label. But what about the delay slot? We can make an else statement and place it there.<br>
    It doesn't feel like it belongs there, though. We'll make a better use of that else statement later.<br>
    That done, let's check the pseudocode:
        ```c
        void exh_mbar_time_out(void) {
            v1 = (s32)a0->unk32C;
            v0 = (s32)a0->unk28;
            a1 = -v1;
            if (v0 != 0) {
                if (v1 == 0) {
                    a1 = 0;
                    goto l_2c;
                } else {
                    v1 = (u8)a0->unk2C;
                }
                v0 = (u8)a0->unk330;
                v0 = v0 ^ v1;
                if (v0 == 0) {
                    a1 = 0;
                }
            }
        l_2c:
            v0 = a1;
            return;
        }
        ```
        Looking better!
  - `goto l_2c;`
    - We're almost there, there's only one goto left.<br>
      On a first glance it looks confusing, but let's see it from another point of view. This goto is two blocks deep in the control flow, which jumps out of those two blocks.<br>
      Described this way, this seems like the block just ends and also gets back from the other block. But if that was the case, you could just remove the goto, right? We can't just remove it given the code inside the first block, though the second block makes a goto out of the two blocks, and the possibility of it being an actual goto is very small. It's almost as if that code only executed if the `(v1 == 0)` condition wasn't true, hm.
        ```c
        void exh_mbar_time_out(void) {
            v1 = (s32)a0->unk32C;
            v0 = (s32)a0->unk28;
            a1 = -v1;
            if (v0 != 0) {
                if (v1 == 0) {
                    a1 = 0;
                } else {
                    v1 = (u8)a0->unk2C;
                    v0 = (u8)a0->unk330;
                    v0 = v0 ^ v1;
                    if (v0 == 0) {
                        a1 = 0;
                    }
                }
            }
            v0 = a1;
            return;
        }
        ```
          
Now, it doesn't look that confusing anymore, does it?<br>
We've successfully removed all fake gotos and restored the original control flow. Congrats!

Though, while the control flow is much better, the pseudocode is still looking ugly.<br>
We just need to do some cleaning, that's all. This is where we start using the debugging info (if there isn't debugging info, this becomes merely guesswork), so let's have a look:
```c
static int exh_mbar_time_out(/* a0 4 */ EXAM_CHECK *ec_pp) {
    /* a1 5 */ int ret;
}
```

We know there's only a real variable, `ret`, mapped to the `$a1` register. There are cases where the register isn't always mapped to that variable, so there is still some guesswork to do. There are also cases where variables are optimized out and won't appear on the debug information. In this case however, the `$a1` register is always mapped to the `ret` variable.

Afterwards, we can get rid of unnecessary variables, and knowing `$a0` is a pointer to a `EXAM_CHECK` object, we can name fields properly. Since we know these field types, we can get rid of the explicit casts as well.<br>
Now, our function will look like this:
```c
void exh_mbar_time_out(void) {
    ret = -a0->oth_num;
    if (a0->ted_num != 0) {
        if (a0->oth_num == 0) {
            ret = 0;
        } else {
            if ((ec_pp->oth[0].th_num ^ ec_pp->ted[0].th_num) == 0) {
                ret = 0;
            }
        }
    }
    v0 = ret;
    return;
}
```

It's slowly taking shape! But what's that strange move to `v0`? And where does `a0` come from?
This code was compiled following the MIPS EABI, where `$v0` is used as a return register, and `$a0` is used as the first argument. But of course, we already know that thanks to the debugging info.

So let's apply that, and merge the separate else and if statements into an 'else if' statement:
```c
static int exh_mbar_time_out(EXAM_CHECK *ec_pp) {
    int ret = -ec_pp->oth_num;
    if (ec_pp->ted_num != 0) {
        if (ec_pp->oth_num == 0) {
            ret = 0;
        } else if ((ec_pp->oth[0].th_num ^ ec_pp->ted[0].th_num) == 0) {
            ret = 0;
        }
    }
    return ret;
}
```

At last, it finally looks like a function, and it matches! But something looks odd.<br>
That XOR check is weird--could it be that the developers didn't actually write a XOR?
Let's examine it further:
  - We have two unsigned bytes, XOR'd and checked if the result is zero.
  - The XOR will only result into a zero when both values match.

This is an optimization made by the compiler, where it avoided doing branches in favor of a XOR and a conditional move.
```c
static int exh_mbar_time_out(EXAM_CHECK *ec_pp) {
    int ret = -ec_pp->oth_num;
    if (ec_pp->ted_num != 0) {
        if (ec_pp->oth_num == 0) {
            ret = 0;
        } else if (ec_pp->oth[0].th_num == ec_pp->ted[0].th_num) {
            ret = 0;
        }
    }
    return ret;
}
```

If we replace that XOR check, our function still matches.<br>
Now that we matched our function, we can move on to the next!.

## Making a decomp.me scratch

If you need some help with a function or you just want to decomp from the browser (the site does have a very nice-looking diffing view), you'll want to make a decomp(dot)me scratch:

1. Obtain the disassembly:
    - It is recommendable that you produce a disassembly with line info to make the decompilation process easier, using the following command as an example:
        - `./tools/add_lines_to_asm.py --asm-path asm/nonmatchings/main/scrctrl/exh_nombar_sub.s --stdump-json-path dump/sym/p3.json --asm-out dump/out.asm --offset 10`
        - It is also recommended to keep the offset at 10 or close values.
    - Second way (no line numbers):
        - Go to the `dump/disasm` folder and find the function you want to decompile on the `{NAME}.text.s` file.
        - If necessary, grab the required data from `.rodata` (e.g. strings or jumptables) in order to have a working m2c decompilation.
2. Go to the decomp.me website and make a [new scratch](https://decomp.me/new), using the `PaRappa the Rapper 2` preset.
    - Use the `PaRappa the Rapper 2 (C++)` preset if the function comes from [prlib](../src/prlib) (C++ library).
3. Paste the disassembly into the 'Target assembly' box.
4. Fill the necessary context. You can always modify it later if unsure. You can start with this context with the base types and defines.
    - ```c
      typedef unsigned char u_char;
      typedef unsigned short u_short;
      typedef unsigned int u_int;
      typedef unsigned long u_long;
      
      #define NULL (0)
      
      #define TRUE (1)
      #define FALSE (0)
      ```
5. Click the 'Create scratch' button and get decompiling!
