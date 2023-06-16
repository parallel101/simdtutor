from manim import *


n = 4
endian = LEFT
hwps = dict(height=1, width=2.5)
opacities = [0.3, 0.45, 0.65, 0.85]


class SetEPI32(Scene):
    def construct(self):
        title = Text("_mm_set_epi32")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(FadeOut(title, run_time=0.25))

        arg_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        arg_texts = Group(*[MathTex('xyzw'[i]).move_to(arg_rects[i]) for i in range(n)])
        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)

        self.play(AnimationGroup(
            FadeIn(arg_texts),
            FadeIn(v1_rects),
        ))
        self.wait(0.55)

        for i in range(n):
            self.play(arg_texts[i].animate.move_to(v1_rects[i]), run_time=0.75)
        self.wait(1)


class Set1EPI32(Scene):
    def construct(self):
        title = Text("_mm_set1_epi32")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(FadeOut(title, run_time=0.25))

        arg_texts = Group(*[MathTex('x').shift(1.25 * DOWN) for i in range(n)])
        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)

        self.play(AnimationGroup(
            FadeIn(arg_texts),
            FadeIn(v1_rects),
        ))
        self.wait(0.85)

        for i in range(n):
            self.play(arg_texts[i].animate.move_to(v1_rects[i]), run_time=0.75)
        self.wait(1)


class SetzeroEPI32(Scene):
    def construct(self):
        title = Text("_mm_setzero_epi32")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(FadeOut(title, run_time=0.25))

        arg_texts = Group(*[MathTex('0').shift(1.25 * DOWN) for i in range(n)])
        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)

        self.play(AnimationGroup(
            FadeIn(arg_texts),
            FadeIn(v1_rects),
        ))
        self.wait(0.8)

        for i in range(n):
            self.play(arg_texts[i].animate.move_to(v1_rects[i]), run_time=0.75)
        self.wait(1)


class AddEPI32(Scene):
    def construct(self):
        title = Text("_mm_add_epi32")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(FadeOut(title, run_time=0.25))

        v1 = np.array([1, 2, 3, 4])
        v2 = np.array([5, 6, 7, 8])
        res = v1 + v2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v1_texts = Group(*[Text(str(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        res_texts = Group(*[Text(str(res[i])).move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(1)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=1, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=1, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=1, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=1, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.5),
            FadeOut(v2_texts, run_time=2.5),
            FadeIn(res_texts, run_time=2.5),
        ))
        self.wait(1)


class ShuffleEPI32(Scene):
    def construct(self):
        title = Text("_mm_shuffle_epi32")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(FadeOut(title, run_time=0.25))

        shuf = [3, 1, 0, 2]

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(1.5 * UP)
        imm_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(len(shuf))]).arrange(endian, buff=0.5).shift(2.25 * DOWN)
        imm_texts = Group(*[Text(str(shuf[i])).move_to(imm_rects[i]) for i in range(len(shuf))])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(imm_texts),
        ))
        self.wait(0.5)

        objs = []
        for i in range(n):
            self.play(imm_texts[i].animate.shift(0.25 * UP), run_time=0.25)
            self.play(imm_texts[i].animate.shift(0.25 * DOWN), run_time=0.25)
            self.wait(0.5)

            obj = v1_rects[shuf[i]].copy()
            self.play(FadeIn(obj, run_time=0.1))
            self.play(obj.animate.shift(0.25 * UP), run_time=0.5)
            self.play(obj.animate.move_to(imm_texts[i]))
            self.wait(0.5)
            objs.append(obj)
        objs = Group(*objs)

        self.play(FadeOut(v1_rects, run_time=0.5))
        self.play(FadeOut(imm_texts, run_time=0.5))
        self.play(objs.animate(lag_ratio=0.5, run_time=1).move_to(ORIGIN))
        self.wait(1)
