from typing import Sequence
from manim import *


n = 4
endian: Sequence[float] = LEFT  # type: ignore
hwps = dict(height=1, width=2.5)
opacities = [0.3, 0.45, 0.65, 0.85]
opacities2 = [0.27, 0.3, 0.37, 0.45, 0.55, 0.65, 0.75, 0.85]
opacities4 = [0.265, 0.28, 0.3, 0.33, 0.37, 0.41, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.89]
opacities8 = [0.26, 0.27, 0.28, 0.29, 0.3, 0.315, 0.33, 0.35, 0.37, 0.39, 0.41, 0.43, 0.45, 0.47, 0.5, 0.525, 0.55, 0.575, 0.6, 0.625, 0.65, 0.675, 0.7, 0.725, 0.75, 0.785, 0.8, 0.825, 0.85, 0.875, 0.89, 0.915]


class SetEPI32(Scene):
    def construct(self):
        title = Text("_mm_set_epi32")
        title2 = Text("单独设置 4 个 int32 分量", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

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
        title2 = Text("全部 int32 元素设为同一个值", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        arg_texts = Group(*[MathTex('x').shift(1.25 * DOWN) for i in range(n)])
        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)

        self.play(AnimationGroup(
            FadeIn(arg_texts),
            FadeIn(v1_rects),
        ))
        self.wait(0.85)

        for i in range(n):
            self.play(arg_texts[i].animate.move_to(v1_rects[i]), run_time=0.7)
            self.wait(0.1)
        self.wait(1)


class SetzeroSI128(Scene):
    def construct(self):
        title = Text("_mm_setzero_si128")
        title2 = Text("全部元素设为零", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

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
        title2 = Text("逐元素 int32 加法", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1 = np.array([1, 2, 3, 4])
        v2 = np.array([5, 6, 7, 8])
        res = v1 + v2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v1_texts = Group(*[Text(str(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        res_texts = Group(*[Text(str(res[i])).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text("+").move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=1.25),
            FadeOut(v2_texts, run_time=1.25),
            FadeIn(res_texts, run_time=1.25),
        ))

        self.wait(1)


class AddEPI16(Scene):
    def construct(self):
        title = Text("_mm_add_epi16")
        title2 = Text("逐元素 int16 加法", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(1)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1 = np.array([500, 1200, -300, -1500, -600, -700, -50, 800], dtype=np.int16) * 40
        v2 = np.array([-40, 840, -480, -720, 90, -1000, 20, -960], dtype=np.int16) * 50
        res1 = (v1.astype(np.int32) + v2.astype(np.int32)).astype(np.int32)
        res = res1.astype(np.int16)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.7
        n = globals()['n'] * 2
        opacities = opacities2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        v1_texts = Group(*[Text(str(v1[i]), font_size=34).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.25).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i]), font_size=34).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.25)
        res1_texts = Group(*[Text(str(res1[i]), font_size=34).move_to(res_rects[i]) for i in range(n)])
        res_texts = Group(*[Text(str(res[i]), font_size=34).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text("+").move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=1.25),
            FadeOut(v2_texts, run_time=1.25),
            FadeIn(res1_texts, run_time=1.25),
        ))

        self.wait(0.6)
        animations = []
        for i in range(n):
            if res[i] != res1[i]:
                animations.append(res1_texts[i].animate.shift(0.25 * UP))
        self.play(AnimationGroup(*animations), run_time=0.25)
        self.wait(0.2)
        animations = []
        for i in range(n):
            if res[i] != res1[i]:
                animations.append(res1_texts[i].animate.shift(0.25 * DOWN))
        self.play(AnimationGroup(*animations), run_time=0.25)

        self.wait(0.8)
        self.play(AnimationGroup(
            FadeOut(res1_texts, run_time=1.5),
            FadeIn(res_texts, run_time=1.5),
        ))
        self.wait(1)


class AddsEPU16(Scene):
    def construct(self):
        title = Text("_mm_adds_epu16")
        title2 = Text("饱和 uint16 加法", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(1)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1 = np.array([500, 1200, 300, 100, 600, 700, 50, 800], dtype=np.uint16) * 40
        v2 = np.array([40, 240, 480, 720, 90, 1000, 20, 960], dtype=np.uint16) * 50
        res1 = (v1.astype(np.uint32) + v2.astype(np.uint32)).astype(np.uint32)
        res = np.minimum(np.maximum(res1, 0), 65535).astype(np.uint16)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.7
        n = globals()['n'] * 2
        opacities = opacities2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        v1_texts = Group(*[Text(str(v1[i]), font_size=34).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.25).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i]), font_size=34).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.25)
        res1_texts = Group(*[Text(str(res1[i]), font_size=34).move_to(res_rects[i]) for i in range(n)])
        res_texts = Group(*[Text(str(res[i]), font_size=34).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text("+").move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=1.25),
            FadeOut(v2_texts, run_time=1.25),
            FadeIn(res1_texts, run_time=1.25),
        ))

        self.wait(0.6)
        animations = []
        for i in range(n):
            if res[i] != res1[i]:
                animations.append(res1_texts[i].animate.shift(0.25 * UP))
        self.play(AnimationGroup(*animations), run_time=0.25)
        self.wait(0.2)
        animations = []
        for i in range(n):
            if res[i] != res1[i]:
                animations.append(res1_texts[i].animate.shift(0.25 * DOWN))
        self.play(AnimationGroup(*animations), run_time=0.25)

        self.wait(0.8)
        self.play(AnimationGroup(
            FadeOut(res1_texts, run_time=1.5),
            FadeIn(res_texts, run_time=1.5),
        ))
        self.wait(1)


class SubsEPU16(Scene):
    def construct(self):
        title = Text("_mm_subs_epu16")
        title2 = Text("饱和 uint16 减法", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(1)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1 = np.array([500, 1200, 300, 100, 600, 700, 50, 800], dtype=np.uint16) * 40
        v2 = np.array([40, 240, 480, 720, 90, 1000, 20, 960], dtype=np.uint16) * 50
        res1 = (v1.astype(np.int32) - v2.astype(np.int32)).astype(np.int32)
        res = np.minimum(np.maximum(res1, 0), 65535).astype(np.uint16)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.7
        n = globals()['n'] * 2
        opacities = opacities2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        v1_texts = Group(*[Text(str(v1[i]), font_size=34).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.25).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i]), font_size=34).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.25)
        res1_texts = Group(*[Text(str(res1[i]), font_size=34).move_to(res_rects[i]) for i in range(n)])
        res_texts = Group(*[Text(str(res[i]), font_size=34).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text("-").move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=1.25),
            FadeOut(v2_texts, run_time=1.25),
            FadeIn(res1_texts, run_time=1.25),
        ))

        self.wait(0.6)
        animations = []
        for i in range(n):
            if res[i] != res1[i]:
                animations.append(res1_texts[i].animate.shift(0.25 * UP))
        self.play(AnimationGroup(*animations), run_time=0.25)
        self.wait(0.2)
        animations = []
        for i in range(n):
            if res[i] != res1[i]:
                animations.append(res1_texts[i].animate.shift(0.25 * DOWN))
        self.play(AnimationGroup(*animations), run_time=0.25)

        self.wait(0.8)
        self.play(AnimationGroup(
            FadeOut(res1_texts, run_time=1.5),
            FadeIn(res_texts, run_time=1.5),
        ))
        self.wait(1)


class AddsEPI16(Scene):
    def construct(self):
        title = Text("_mm_adds_epi16")
        title2 = Text("饱和 int16 加法", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(1)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1 = np.array([500, 1200, -300, -1500, -600, -700, -50, 800], dtype=np.int16) * 40
        v2 = np.array([-40, 840, -480, -720, 90, -1000, 20, -960], dtype=np.int16) * 50
        res1 = (v1.astype(np.int32) + v2.astype(np.int32)).astype(np.int32)
        res = np.minimum(np.maximum(res1, -32768), 32767).astype(np.int16)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.7
        n = globals()['n'] * 2
        opacities = opacities2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        v1_texts = Group(*[Text(str(v1[i]), font_size=34).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.25).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i]), font_size=34).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.25)
        res1_texts = Group(*[Text(str(res1[i]), font_size=34).move_to(res_rects[i]) for i in range(n)])
        res_texts = Group(*[Text(str(res[i]), font_size=34).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text("+").move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=1.25),
            FadeOut(v2_texts, run_time=1.25),
            FadeIn(res1_texts, run_time=1.25),
        ))

        self.wait(0.6)
        animations = []
        for i in range(n):
            if res[i] != res1[i]:
                animations.append(res1_texts[i].animate.shift(0.25 * UP))
        self.play(AnimationGroup(*animations), run_time=0.25)
        self.wait(0.2)
        animations = []
        for i in range(n):
            if res[i] != res1[i]:
                animations.append(res1_texts[i].animate.shift(0.25 * DOWN))
        self.play(AnimationGroup(*animations), run_time=0.25)

        self.wait(0.8)
        self.play(AnimationGroup(
            FadeOut(res1_texts, run_time=1.5),
            FadeIn(res_texts, run_time=1.5),
        ))
        self.wait(1)


class AvgEPU16(Scene):
    def construct(self):
        title = Text("_mm_avg_epu16")
        title2 = Text("uint16 逐元素相加求平均", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(1)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1 = np.array([50, 100, 200, 300, 500, 600, 700, 800], dtype=np.uint16)
        v2 = np.array([20, 40, 80, 120, 240, 480, 720, 960], dtype=np.uint16)
        res1 = (v1.astype(np.uint32) + v2.astype(np.uint32)).astype(np.uint32)
        res2 = (res1 + 1).astype(np.uint32)
        res = (res2 // 2).astype(np.uint16)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.7
        n = globals()['n'] * 2
        opacities = opacities2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        v1_texts = Group(*[Text(str(v1[i]), font_size=38).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.25).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i]), font_size=38).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.25)
        res1_texts = Group(*[Text(str(res1[i]), font_size=38).move_to(res_rects[i]) for i in range(n)])
        res2_texts = Group(*[Text(str(res2[i]), font_size=38).move_to(res_rects[i]) for i in range(n)])
        res_texts = Group(*[Text(str(res[i]), font_size=38).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text("+").move_to(res_rects[i]) for i in range(n)])
        opa_texts = Group(*[Text("+1").move_to(res_rects[i]).shift(DOWN * 0.8) for i in range(n)])
        opb_texts = Group(*[Text("/2").move_to(res_rects[i]).shift(DOWN * 0.8) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res1_texts, run_time=2.25),
        ))
        self.wait(0.4)
        self.play(FadeIn(opa_texts))
        self.wait(0.7)
        self.play(FadeOut(opa_texts))
        self.play(AnimationGroup(
            FadeOut(res1_texts, run_time=1.5),
            FadeIn(res2_texts, run_time=1.5),
        ))
        self.wait(0.4)
        self.play(FadeIn(opb_texts))
        self.wait(0.7)
        self.play(FadeOut(opb_texts))
        self.play(AnimationGroup(
            FadeOut(res2_texts, run_time=1.5),
            FadeIn(res_texts, run_time=1.5),
        ))
        self.wait(1)


class HaddEPI16(Scene):
    def construct(self):
        title = Text("_mm_hadd_epi16")
        title2 = Text("两个寄存器的 16 个 int16 水平相加，得到 8 个 int16 组成的矢量", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.7
        n = globals()['n'] * 2
        opacities = opacities2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.25).shift(DOWN)
        v1_rects_old = v1_rects.copy()
        v2_rects_old = v2_rects.copy()

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
        ))
        self.wait(0.7)

        ag = []
        for i in range(n):
            if i % 2 != 0:
                ag.append(v1_rects[i].animate.move_to(v1_rects[i - 1]))
        self.play(AnimationGroup(*ag))
        self.wait(0.4)
        ag = []
        for i in range(n):
            if i % 2 != 0:
                ag.append(FadeOut(v1_rects[i]))
        self.play(AnimationGroup(*ag))
        self.wait(0.5)

        ag = []
        for i in range(n):
            if i % 2 != 0:
                ag.append(v2_rects[i].animate.move_to(v2_rects[i - 1]))
        self.play(AnimationGroup(*ag))
        self.wait(0.4)
        ag = []
        for i in range(n):
            if i % 2 != 0:
                ag.append(FadeOut(v2_rects[i]))
        self.play(AnimationGroup(*ag))
        self.wait(0.5)

        ag = []
        for i in range(n):
            if i % 2 == 0 and i != 0:
                ag.append(v1_rects[i].animate.move_to(v1_rects_old[i // 2]))
        for i in range(n):
            if i % 2 == 0 and i != 0:
                ag.append(v2_rects[i].animate.move_to(v2_rects_old[i // 2]))
        self.play(AnimationGroup(*ag))
        self.wait(0.5)

        for i in range(n):
            if i % 2 == 0:
                ag.append(v1_rects[i].animate.shift(DOWN))
                ag.append(v2_rects[i].animate.move_to(v1_rects_old[n // 2 + i // 2].shift(DOWN)))
        self.play(AnimationGroup(*ag))
        self.wait(1)


class AbsEPI16(Scene):
    def construct(self):
        title = Text("_mm_abs_epi16")
        title2 = Text("8 个 int16 逐个求绝对值", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(1)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 3.35 / 2
        n = globals()['n'] * 2
        opacities = opacities2

        v1 = np.array([500, -1200, 300, -1500, -600, 700, -50, 800], dtype=np.int16) // 10
        res = np.abs(v1.astype(np.int16)).astype(np.uint8)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.1)
        v1_texts = Group(*[Text(str(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        res_texts = Group(*[Text(str(res[i])).move_to(v1_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.1)

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
        ))

        self.wait(0.8)
        animations = []
        for i in range(n):
            if res[i] != v1[i]:
                animations.append(v1_texts[i].animate.shift(0.25 * UP))
        self.play(AnimationGroup(*animations), run_time=0.25)
        self.wait(0.2)
        animations = []
        for i in range(n):
            if res[i] != v1[i]:
                animations.append(v1_texts[i].animate.shift(0.25 * DOWN))
        self.play(AnimationGroup(*animations), run_time=0.25)

        self.wait(0.8)
        self.play(Transform(v1_texts, res_texts), run_time=1.5)
        self.wait(1)


class SadEPU8(Scene):
    def construct(self):
        title = Text("_mm_sad_epu8")
        title2 = Text("2x16 个 uint8，8 个一组，每组内逐元素求差的绝对值水平求和得到 2 个 uint16", font_size=28).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(1)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 3.35
        hwps2 = dict(**hwps)
        hwps2['width'] *= 2
        n = globals()['n'] * 4
        opacities = opacities4

        v1 = np.array([157,10,20,250, 50,60,7,80, 72,98,13,3, 72,164,3,49], dtype=np.uint8)
        v2 = np.array([15,40,8,12, 24,148,72,96, 11,184,213,48, 11,44,91,26], dtype=np.uint8)
        res1 = v1.astype(np.int16) - v2.astype(np.int16)
        res = np.abs(res1.astype(np.int16)).astype(np.uint8)
        res2 = np.array([np.sum(res[i:i + 8].astype(np.uint16)).astype(np.uint16) if i % 4 == 0 else 0 for i in range(n // 2)])

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.1).shift(UP)
        v1_texts = Group(*[Text(str(v1[i]), font_size=32).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.1).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i]), font_size=32).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=YELLOW if (i // 8) % 2 == 0 else PURPLE) for i in range(n)]).arrange(endian, buff=0.1)
        res2_rects = Group(*[Rectangle(**hwps2, fill_opacity=opacities[i], fill_color=YELLOW if (i // 4) % 2 == 0 else PURPLE) for i in range(n // 2)]).arrange(endian, buff=0.2)
        res1_texts = Group(*[Text(str(res1[i]), font_size=32).move_to(res_rects[i]) for i in range(n)])
        res_texts = Group(*[Text(str(res[i]), font_size=32).move_to(res_rects[i]) for i in range(n)])
        res2_texts = Group(*[Text(str(res2[i])).move_to(res2_rects[i]) for i in range(n // 2)])
        op_texts = Group(*[Text("-").move_to(res_rects[i]) for i in range(n)])
        op2_texts = Group(*[Text("+", font_size=32).move_to(res_rects[i]).shift((0.1 / 2 + hwps['width'] / 2) * RIGHT) for i in range(n - 1) if i % 8 != 0])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))

        self.wait(0.75)
        self.play(AnimationGroup(
            v1_texts.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
            v1_rects.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
        ))
        self.wait(0.4)
        self.play(AnimationGroup(
            FadeOut(v1_rects),
            FadeOut(v2_rects),
            FadeOut(v1_texts),
            FadeOut(v2_texts),
            FadeIn(res_rects),
            FadeIn(res1_texts),
        ), run_time=1.8)

        self.wait(0.6)
        animations = []
        for i in range(n):
            if res[i] != res1[i]:
                animations.append(res1_texts[i].animate.shift(0.25 * UP))
        self.play(AnimationGroup(*animations), run_time=0.25)
        self.wait(0.2)
        animations = []
        for i in range(n):
            if res[i] != res1[i]:
                animations.append(res1_texts[i].animate.shift(0.25 * DOWN))
        self.play(AnimationGroup(*animations), run_time=0.25)

        self.wait(0.75)
        self.play(Transform(res1_texts, res_texts), run_time=1.5)
        self.wait(1)

        self.play(FadeIn(op2_texts))
        self.wait(0.8)
        self.play(FadeOut(op2_texts))
        self.wait(0.5)
        self.play(Transform(res_rects, res2_rects), Transform(res1_texts, res2_texts), run_time=1.6)
        self.wait(1.5)


class MaddEPI16(Scene):
    def construct(self):
        title = Text("_mm_madd_epi16")
        title2 = Text("2x8 个 int16 逐元素相乘变为 8 个 int32，水平相加得到 4 个 int32", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(1)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        hwps = dict(**globals()['hwps'])
        hwpsx = dict(**globals()['hwps'])
        hwps['width'] /= 1.89
        hwpsx['width'] *= 1.23
        n = globals()['n'] * 2
        opacities = opacities2

        v1 = np.array([1, 2, 3, 4, 5, 6, 7, 8], dtype=np.int16)
        v2 = np.array([1, 9, 6, 9, 2, 0, 2, 3], dtype=np.int16)
        res1 = v1 * v2
        res = np.array([res1[i * 2].astype(np.int32) + res1[i * 2 + 1].astype(np.int32) for i in range(n // 2)])

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.4).shift(UP)
        v1_texts = Group(*[Text(str(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.4).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i])).move_to(v2_rects[i]) for i in range(n)])
        res1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=YELLOW) for i in range(n)]).arrange(endian, buff=0.4)
        res_rects = Group(*[Rectangle(**hwpsx, fill_opacity=globals()['opacities'][i], fill_color=RED) for i in range(n // 2)]).arrange(endian, buff=0.4)
        res1_texts = Group(*[Text(str(res1[i])).move_to(res1_rects[i]) for i in range(n)])
        res_texts = Group(*[Text(str(res[i])).move_to(res_rects[i]) for i in range(n // 2)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))

        op_texts = Group(*[Text("*").move_to(res1_rects[i]) for i in range(n)])
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v2_texts, run_time=1.05),
            FadeOut(v2_rects, run_time=1.05),
            Transform(v1_texts, res1_texts, run_time=1.05),
            Transform(v1_rects, res1_rects, run_time=1.05),
        ))

        op_texts = Group(*[Text("+").move_to(res_rects[i]) for i in range(n // 2)])
        self.wait(0.8)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))

        self.wait(0.7)
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=1.5),
            FadeOut(v1_rects, run_time=1.5),
            FadeIn(res_texts, run_time=1.5),
            FadeIn(res_rects, run_time=1.5),
        ))
        self.wait(1)


# class MaddubsEPI16(Scene):
#     pass # TODO


class MulloEPI16(Scene):
    def construct(self):
        title = Text("_mm_mullo_epi16")
        title2 = Text("int16 乘法，得到的 int32 取低 16 位", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(1)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1 = np.array([50, 100, 200, 300, 500, 600, 700, 800], dtype=np.int16) * 40
        v2 = np.array([20, 40, 80, 120, 240, 480, 720, 960], dtype=np.int16) * 50
        res1 = (v1.astype(np.int32) * v2.astype(np.int32)).astype(np.int32)
        res = (res1.astype(np.uint32)).astype(np.uint16)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.68
        n = globals()['n'] * 2
        opacities = opacities2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        v1_texts = Group(*[Text(str(v1[i]), font_size=32).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.25).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i]), font_size=32).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.25)
        res1_texts = Group(*[Text(str(res1[i]), font_size=20).move_to(res_rects[i]) for i in range(n)])
        res_texts = Group(*[Text(str(res[i]), font_size=32).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text("*").move_to(res_rects[i]) for i in range(n)])
        opa_texts = Group(*[Text("&0xffff", font_size=30).move_to(res_rects[i]).shift(DOWN * 0.8) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res1_texts, run_time=2.25),
        ))
        self.wait(0.4)
        self.play(FadeIn(opa_texts))
        self.wait(0.7)
        self.play(FadeOut(opa_texts))
        self.play(AnimationGroup(
            FadeOut(res1_texts, run_time=1.5),
            FadeIn(res_texts, run_time=1.5),
        ))
        self.wait(1)


class MulhiEPI16(Scene):
    def construct(self):
        title = Text("_mm_mulhi_epi16")
        title2 = Text("int16 乘法，得到的 int32 取高 16 位", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(1)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1 = np.array([50, 100, 200, 300, 500, 600, 700, 800], dtype=np.int16) * 40
        v2 = np.array([20, 40, 80, 120, 240, 480, 720, 960], dtype=np.int16) * 50
        res1 = (v1.astype(np.int32) * v2.astype(np.int32)).astype(np.int32)
        res = (res1.astype(np.int32) >> 16).astype(np.int16)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.68
        n = globals()['n'] * 2
        opacities = opacities2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        v1_texts = Group(*[Text(str(v1[i]), font_size=32).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.25).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i]), font_size=32).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.25)
        res1_texts = Group(*[Text(str(res1[i]), font_size=20).move_to(res_rects[i]) for i in range(n)])
        res_texts = Group(*[Text(str(res[i]), font_size=32).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text("*").move_to(res_rects[i]) for i in range(n)])
        opa_texts = Group(Text("算术右移 (高位按符号位扩展)").shift(2.25 * DOWN), *[Text(">>16", font_size=30).move_to(res_rects[i]).shift(DOWN * 0.8) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res1_texts, run_time=2.25),
        ))
        self.wait(0.4)
        self.play(FadeIn(opa_texts))
        self.wait(0.7)
        self.play(FadeOut(opa_texts))
        self.play(AnimationGroup(
            FadeOut(res1_texts, run_time=1.5),
            FadeIn(res_texts, run_time=1.5),
        ))
        self.wait(1)


class MulhiEPU16(Scene):
    def construct(self):
        title = Text("_mm_mulhi_epu16")
        title2 = Text("uint32 乘法，得到的 uint32 取高 16 位", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(1)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1 = np.array([50, 100, 200, 300, 500, 600, 700, 800], dtype=np.uint16) * 40
        v2 = np.array([20, 40, 80, 120, 240, 480, 720, 960], dtype=np.uint16) * 50
        res1 = (v1.astype(np.uint32) * v2.astype(np.uint32)).astype(np.uint32)
        res = (res1.astype(np.uint32) >> 16).astype(np.uint16)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.68
        n = globals()['n'] * 2
        opacities = opacities2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        v1_texts = Group(*[Text(str(v1[i]), font_size=32).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.25).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i]), font_size=32).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.25)
        res1_texts = Group(*[Text(str(res1[i]), font_size=20).move_to(res_rects[i]) for i in range(n)])
        res_texts = Group(*[Text(str(res[i]), font_size=32).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text("*").move_to(res_rects[i]) for i in range(n)])
        opa_texts = Group(Text("逻辑右移 (高位填充 0)").shift(2.25 * DOWN), *[Text(">>16", font_size=30).move_to(res_rects[i]).shift(DOWN * 0.8) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res1_texts, run_time=2.25),
        ))
        self.wait(0.4)
        self.play(FadeIn(opa_texts))
        self.wait(0.7)
        self.play(FadeOut(opa_texts))
        self.play(AnimationGroup(
            FadeOut(res1_texts, run_time=1.5),
            FadeIn(res_texts, run_time=1.5),
        ))
        self.wait(1)


class MulhrsEPI16(Scene):
    def construct(self):
        title = Text("_mm_mulhrs_epi16")
        title2 = Text("量化的 int16 乘法，结果近似到最近的偶数，常用于音频处理", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(1)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1 = np.array([50, 100, 200, 300, 500, 600, 700, 800], dtype=np.int16) * 40
        v2 = np.array([20, 40, 80, 120, 240, 480, 720, 960], dtype=np.int16) * 50
        res1 = (v1.astype(np.int32) * v2.astype(np.int32)).astype(np.int32)
        res2 = (res1 >> 14).astype(np.int32)
        res3 = (res2 + 1).astype(np.int32)
        res = (res3.astype(np.uint32) >> 1).astype(np.int16)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.68
        n = globals()['n'] * 2
        opacities = opacities2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        v1_texts = Group(*[Text(str(v1[i]), font_size=32).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.25).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i]), font_size=32).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.25)
        res1_texts = Group(*[Text(str(res1[i]), font_size=20).move_to(res_rects[i]) for i in range(n)])
        res2_texts = Group(*[Text(str(res2[i]), font_size=32).move_to(res_rects[i]) for i in range(n)])
        res3_texts = Group(*[Text(str(res3[i]), font_size=32).move_to(res_rects[i]) for i in range(n)])
        res_texts = Group(*[Text(str(res[i]), font_size=32).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text("*").move_to(res_rects[i]) for i in range(n)])
        opa_texts = Group(*[Text(">>14", font_size=36).move_to(res_rects[i]).shift(DOWN * 0.8) for i in range(n)])
        opb_texts = Group(*[Text("+1", font_size=36).move_to(res_rects[i]).shift(DOWN * 0.8) for i in range(n)])
        opc_texts = Group(*[Text(">>1", font_size=36).move_to(res_rects[i]).shift(DOWN * 0.8) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(1)
        self.play(FadeOut(op_texts))
        self.wait(0.4)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=2.25),
            FadeOut(v2_texts, run_time=2.25),
            FadeIn(res1_texts, run_time=2.25),
        ))
        self.wait(0.4)
        self.play(FadeIn(opa_texts))
        self.wait(0.7)
        self.play(FadeOut(opa_texts))
        self.play(AnimationGroup(
            FadeOut(res1_texts, run_time=1.5),
            FadeIn(res2_texts, run_time=1.5),
        ))
        self.wait(0.4)
        self.play(FadeIn(opb_texts))
        self.wait(0.7)
        self.play(FadeOut(opb_texts))
        self.play(AnimationGroup(
            FadeOut(res2_texts, run_time=1.5),
            FadeIn(res3_texts, run_time=1.5),
        ))
        self.wait(0.4)
        self.play(FadeIn(opc_texts))
        self.wait(0.7)
        self.play(FadeOut(opc_texts))
        self.play(AnimationGroup(
            FadeOut(res3_texts, run_time=1.5),
            FadeIn(res_texts, run_time=1.5),
        ))
        self.wait(1)


class MaxEPI16(Scene):
    def construct(self):
        title = Text("_mm_max_epi16")
        title2 = Text("逐元素 int16 求最大值", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(1)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1 = np.array([500, 1200, -300, -1500, -600, -700, -50, 800], dtype=np.int16) * 40
        v2 = np.array([-40, 840, -480, -720, 90, -1000, 20, -960], dtype=np.int16) * 50
        res = np.maximum(v1.astype(np.int16), v2.astype(np.int16)).astype(np.int16)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.7
        n = globals()['n'] * 2
        opacities = opacities2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        v1_texts = Group(*[Text(str(v1[i]), font_size=36).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.25).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i]), font_size=36).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.25)
        res_texts = Group(*[Text(str(res[i]), font_size=36).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text("MAX", font_size=32).move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(0.8)
        self.play(FadeOut(op_texts))

        self.wait(0.6)
        animations = []
        for i in range(n):
            if res[i] == v1[i]:
                animations.append(v1_texts[i].animate.shift(0.25 * UP))
            if res[i] == v2[i]:
                animations.append(v2_texts[i].animate.shift(0.25 * UP))
        self.play(AnimationGroup(*animations), run_time=0.25)
        self.wait(0.2)
        animations = []
        for i in range(n):
            if res[i] == v1[i]:
                animations.append(v1_texts[i].animate.shift(0.25 * DOWN))
            if res[i] == v2[i]:
                animations.append(v2_texts[i].animate.shift(0.25 * DOWN))
        self.play(AnimationGroup(*animations), run_time=0.25)
        self.wait(0.8)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=1.25),
            FadeOut(v2_texts, run_time=1.25),
            FadeIn(res_texts, run_time=1.25),
        ))
        self.wait(1)


class MinEPU16(Scene):
    def construct(self):
        title = Text("_mm_min_epu16")
        title2 = Text("逐元素 uint16 求最小值", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(1)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1 = np.array([500, 1200, -300, -1500, -600, -700, -50, 800], dtype=np.uint16) * 40
        v2 = np.array([-40, 840, -480, -720, 90, -1000, 20, -960], dtype=np.uint16) * 50
        res = np.minimum(v1.astype(np.uint16), v2.astype(np.uint16)).astype(np.uint16)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.7
        n = globals()['n'] * 2
        opacities = opacities2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        v1_texts = Group(*[Text(str(v1[i]), font_size=36).move_to(v1_rects[i]) for i in range(n)])
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.25).shift(DOWN)
        v2_texts = Group(*[Text(str(v2[i]), font_size=36).move_to(v2_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.25)
        res_texts = Group(*[Text(str(res[i]), font_size=36).move_to(res_rects[i]) for i in range(n)])
        op_texts = Group(*[Text("MIN", font_size=32).move_to(res_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(v2_rects),
            FadeIn(v2_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_texts))
        self.wait(0.8)
        self.play(FadeOut(op_texts))

        self.wait(0.6)
        animations = []
        for i in range(n):
            if res[i] == v1[i]:
                animations.append(v1_texts[i].animate.shift(0.25 * UP))
            if res[i] == v2[i]:
                animations.append(v2_texts[i].animate.shift(0.25 * UP))
        self.play(AnimationGroup(*animations), run_time=0.25)
        self.wait(0.2)
        animations = []
        for i in range(n):
            if res[i] == v1[i]:
                animations.append(v1_texts[i].animate.shift(0.25 * DOWN))
            if res[i] == v2[i]:
                animations.append(v2_texts[i].animate.shift(0.25 * DOWN))
        self.play(AnimationGroup(*animations), run_time=0.25)
        self.wait(0.8)

        self.play(AnimationGroup(
            v1_rects.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_rects.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
            v1_texts.animate(lag_ratio=0.4, run_time=2.5).shift(DOWN),
            v2_texts.animate(lag_ratio=0.4, run_time=2.5).shift(UP),
        ))
        self.play(AnimationGroup(
            FadeOut(v1_texts, run_time=1.25),
            FadeOut(v2_texts, run_time=1.25),
            FadeIn(res_texts, run_time=1.25),
        ))
        self.wait(1)


class MinposEPU16(Scene): # TODO
    def construct(self):
        title = Text("_mm_minpos_epu16")
        title2 = Text("求 8 个 uint16 的最小值（水平），并记录最小值所在的下标（0到7）", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(1)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1 = np.array([500, 1200, 300, 1500, 600, 700, 300, 800], dtype=np.uint16) // 10
        res = np.argmin(v1.astype(np.uint16))

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.7
        n = globals()['n'] * 2
        opacities = opacities2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        v1_texts = Group(*[Text(str(v1[i])).move_to(v1_rects[i]) for i in range(n)])
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.25)
        res_texts = Group(*[Text(str(res if i == 1 else v1[res] if i == 0 else 0)).move_to(res_rects[i]) for i in range(n)])
        ind_texts = Group(*[Text(str(i)).move_to(res_rects[i]).shift(DOWN * 0.65) for i in range(n)])
        op_text = Text("ARGMIN").shift(DOWN * 1.95)

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
            FadeIn(ind_texts),
        ))
        self.wait(0.75)
        self.play(FadeIn(op_text))
        self.wait(0.8)
        self.play(FadeOut(op_text))

        self.wait(0.6)
        animations = []
        animations.append(v1_texts[res].animate.shift(0.25 * UP))
        animations.append(ind_texts[res].animate.shift(0.25 * UP))
        self.play(AnimationGroup(*animations), run_time=0.25)
        self.wait(0.2)
        animations = []
        animations.append(v1_texts[res].animate.shift(0.25 * DOWN))
        animations.append(ind_texts[res].animate.shift(0.25 * DOWN))
        self.play(AnimationGroup(*animations), run_time=0.25)
        self.wait(0.8)

        animations = []
        for i in range(n):
            if i != res:
                animations.append(FadeOut(ind_texts[i]))
                animations.append(FadeOut(v1_texts[i]))
                animations.append(FadeOut(v1_rects[i]))
        self.play(AnimationGroup(*animations))

        animations = []
        animations.append(Transform(v1_texts[res], res_texts[0]))
        animations.append(Transform(v1_rects[res], res_rects[0]))
        animations.append(Transform(ind_texts[res], res_texts[1]))
        self.play(AnimationGroup(*animations), run_time=1.4)

        animations = []
        for i in range(1, n):
            animations.append(FadeIn(res_rects[i]))
        for i in range(2, n):
            animations.append(FadeIn(res_texts[i]))
        self.play(AnimationGroup(*animations))

        self.wait(1)


class ShuffleEPI32(Scene):
    def construct(self):
        title = Text("_mm_shuffle_epi32")
        title2 = Text("给寄存器中的 4 个 int32 重排序，顺序通过立即数指定", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

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


class ShuffleEPI8(Scene):
    def construct(self):
        title = Text("_mm_shuffle_epi8")
        title2 = Text("给寄存器中的 16 个 int8 重排序，顺序通过立即数指定", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 3.5
        n = globals()['n'] * 4
        opacities = opacities4

        shuf = [3, 7, 1, 8, 9, 15, 11, 0, 14, 6, 2, 12, 10, 5, 4, 13]

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.15).shift(1.5 * UP)
        imm_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(len(shuf))]).arrange(endian, buff=0.15).shift(2.25 * DOWN)
        imm_texts = Group(*[Text(str(shuf[i]), font_size=36).move_to(imm_rects[i]) for i in range(len(shuf))])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(imm_texts),
        ))
        self.wait(0.5)

        objs = []
        for i in range(n):
            self.play(imm_texts[i].animate.shift(0.25 * UP), run_time=0.05)
            self.play(imm_texts[i].animate.shift(0.25 * DOWN), run_time=0.05)

            obj = v1_rects[shuf[i]].copy()
            self.play(FadeIn(obj, run_time=0.04))
            self.play(obj.animate.shift(0.25 * UP), run_time=0.08)
            self.play(obj.animate.move_to(imm_texts[i]), run_time=0.13)
            objs.append(obj)
        objs = Group(*objs)

        self.play(FadeOut(v1_rects, run_time=0.5))
        self.play(FadeOut(imm_texts, run_time=0.5))
        self.play(objs.animate(lag_ratio=0.5, run_time=1).move_to(ORIGIN))
        self.wait(1)


class AlignrEPI8(Scene):
    def construct(self):
        title = Text("_mm_alignr_epi8")
        title2 = Text("两 128 位寄存器拼接后按位右移，一次移 8 位，得到结果取最低 128 位", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 6.75
        n = globals()['n'] * 4
        opacities = opacities4

        offset = 5

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.06).shift(1.25 * UP)
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.06).shift(1.25 * DOWN)
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i % n], fill_color=RED) for i in range(n * 2)]).arrange(endian, buff=0.06)
        imm_text = Text(str(offset)).shift(2.4 * DOWN)
        zero_texts = Group(*[Text('0').move_to(res_rects[i]) for i in range(len(res_rects))])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
            FadeIn(imm_text),
        ))
        self.wait(0.6)

        animations = []
        for i in range(n):
            animations.append(v1_rects[i].animate.move_to(res_rects[i + n]))
            animations.append(v2_rects[i].animate.move_to(res_rects[i]))
        self.play(AnimationGroup(*animations), run_time=1.5)
        self.wait(0.8)

        self.play(imm_text.animate.shift(0.25 * UP), run_time=0.5)
        self.play(imm_text.animate.shift(0.25 * DOWN), run_time=0.5)
        self.wait(0.5)
        animations = []
        animations1 = []
        animations2 = []
        animations3 = []
        for i in range(n):
            if i - offset < 0:
                animations1.append(FadeOut(v2_rects[i]))
            else:
                animations.append(v2_rects[i].animate.move_to(res_rects[i - offset]))
            if i + n - offset < 0:
                animations1.append(FadeOut(v1_rects[i]))
            else:
                animations.append(v1_rects[i].animate.move_to(res_rects[i + n - offset]))
        self.play(AnimationGroup(*animations1), run_time=1.0)
        self.play(AnimationGroup(*animations), run_time=1.5)
        for i in range(n):
            if i + n - offset >= n:
                animations2.append(FadeOut(v1_rects[i]))
            else:
                animations3.append(v1_rects[i].animate.move_to(res_rects[n // 2 + i + n - offset]))
            if i - offset >= 0:
                animations3.append(v2_rects[i].animate.move_to(res_rects[n // 2 + i - offset]))
        self.play(AnimationGroup(*animations2), run_time=1.0)
        self.play(AnimationGroup(*animations3), run_time=0.9)
        self.wait(1)


class BslliSI128(Scene):
    def construct(self):
        title = Text("_mm_bslli_si128")
        title2 = Text("128 位寄存器按位左移，一次移 8 位，低位填充 0", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 3.85
        n = globals()['n'] * 4
        opacities = opacities4

        offset = 5

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.15)
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i % n], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.15)
        imm_text = Text(str(offset)).shift(2.4 * DOWN)
        zero_texts = Group(*[Text('0').move_to(res_rects[i]) for i in range(len(res_rects))])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(imm_text),
        ))
        self.wait(0.8)

        self.play(imm_text.animate.shift(0.25 * UP), run_time=0.5)
        self.play(imm_text.animate.shift(0.25 * DOWN), run_time=0.5)
        self.wait(0.5)

        animations = []
        animations1 = []
        animations2 = []
        for i in range(n):
            if i + offset >= n:
                animations1.append(FadeOut(v1_rects[i]))
            else:
                animations.append(v1_rects[i].animate.move_to(res_rects[i + offset]))
        for i in range(offset):
            animations2.append(FadeIn(res_rects[i]))
            animations2.append(FadeIn(zero_texts[i]))
        self.play(AnimationGroup(*animations1), run_time=1.0)
        self.play(AnimationGroup(*animations), run_time=1.5)
        self.play(AnimationGroup(*animations2), run_time=0.8)
        self.wait(1)


class BsrliSI128(Scene):
    def construct(self):
        title = Text("_mm_bsrli_si128")
        title2 = Text("128 位寄存器按位右移，一次移 8 位，高位填充 0", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 3.85
        n = globals()['n'] * 4
        opacities = opacities4

        offset = 5

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.15)
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i % n], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.15)
        imm_text = Text(str(offset)).shift(2.4 * DOWN)
        zero_texts = Group(*[Text('0').move_to(res_rects[i]) for i in range(len(res_rects))])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(imm_text),
        ))
        self.wait(0.8)

        self.play(imm_text.animate.shift(0.25 * UP), run_time=0.5)
        self.play(imm_text.animate.shift(0.25 * DOWN), run_time=0.5)
        self.wait(0.5)

        animations = []
        animations1 = []
        animations2 = []
        for i in range(n):
            if i - offset < 0:
                animations1.append(FadeOut(v1_rects[i]))
            else:
                animations.append(v1_rects[i].animate.move_to(res_rects[i - offset]))
        for i in range(offset):
            animations2.append(FadeIn(res_rects[n - 1 - i]))
            animations2.append(FadeIn(zero_texts[n - 1 - i]))
        self.play(AnimationGroup(*animations1), run_time=1.0)
        self.play(AnimationGroup(*animations), run_time=1.5)
        self.play(AnimationGroup(*animations2), run_time=0.8)
        self.wait(1)


class SrliEPI32(Scene):
    def construct(self):
        title = Text("_mm_srli_epi32")
        title2 = Text("int32 逐个按位右移，位数由立即数指定，高位填充 0", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 3.85
        n = globals()['n'] * 4
        opacities = opacities4

        offset = 5

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.15)
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i % n], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.15)
        imm_text = Text(str(offset)).shift(2.4 * DOWN)
        zero_texts = Group(*[Text('0').move_to(res_rects[i]) for i in range(len(res_rects))])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(imm_text),
        ))
        self.wait(0.8)

        self.play(imm_text.animate.shift(0.25 * UP), run_time=0.5)
        self.play(imm_text.animate.shift(0.25 * DOWN), run_time=0.5)
        self.wait(0.5)

        animations = []
        animations1 = []
        animations2 = []
        for i in range(n):
            if i - offset < 0:
                animations1.append(FadeOut(v1_rects[i]))
            else:
                animations.append(v1_rects[i].animate.move_to(res_rects[i - offset]))
        for i in range(offset):
            animations2.append(FadeIn(res_rects[n - 1 - i]))
            animations2.append(FadeIn(zero_texts[n - 1 - i]))
        self.play(AnimationGroup(*animations1), run_time=1.0)
        self.play(AnimationGroup(*animations), run_time=1.5)
        self.play(AnimationGroup(*animations2), run_time=0.8)
        self.wait(1)


class ExtractEPI16(Scene):
    def construct(self):
        title = Text("_mm_extract_epi16")
        title2 = Text("从 8 个 int16 组成的矢量中提取出指定分量", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.7
        n = globals()['n'] * 2
        opacities = opacities2

        idx = 5

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        arg_texts = Group(*[MathTex('x_' + str(i)).move_to(v1_rects[i]) for i in range(n)])
        imm_text = Text(str(idx)).shift(DOWN)

        self.play(AnimationGroup(
            FadeIn(arg_texts),
            FadeIn(v1_rects),
            FadeIn(imm_text),
        ))
        self.wait(0.75)

        self.play(imm_text.animate.shift(0.2 * UP), run_time=0.2)
        self.play(imm_text.animate.shift(-0.2 * UP), run_time=0.2)
        self.wait(0.45)
        self.play(v1_rects[idx].animate.shift(0.15 * UP), run_time=0.2)
        self.play(v1_rects[idx].animate.shift(-0.15 * UP), run_time=0.2)
        self.wait(0.6)
        self.play(FadeOut(imm_text), arg_texts[idx].copy().animate.move_to(DOWN), run_time=0.75)
        self.wait(1)


class InsertEPI16(Scene):
    def construct(self):
        title = Text("_mm_insert_epi16")
        title2 = Text("将 8 个 int16 组成的矢量的指定分量替换为一个新的 int16 值", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.7
        n = globals()['n'] * 2
        opacities = opacities2

        idx = 5

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        arg_texts = Group(*[MathTex('x_' + str(i)).move_to(v1_rects[i]) for i in range(n)])
        imm_text = Text(str(idx)).shift(DOWN).shift(LEFT)
        ins_text = MathTex('y').shift(DOWN).shift(RIGHT)

        self.play(AnimationGroup(
            FadeIn(arg_texts),
            FadeIn(v1_rects),
            FadeIn(imm_text),
            FadeIn(ins_text),
        ))
        self.wait(0.75)

        self.play(imm_text.animate.shift(0.2 * UP), run_time=0.2)
        self.play(imm_text.animate.shift(-0.2 * UP), run_time=0.2)
        self.wait(0.45)
        self.play(v1_rects[idx].animate.shift(0.15 * UP), run_time=0.2)
        self.play(v1_rects[idx].animate.shift(-0.15 * UP), run_time=0.2)
        self.wait(0.6)
        self.play(FadeOut(arg_texts[idx]), ins_text.copy().animate.move_to(arg_texts[idx]), run_time=0.75)
        self.wait(1)


class PacksEPI32(Scene):
    def construct(self): # TODO
        title = Text("_mm_packs_epi32")
        title2 = Text("两个寄存器中的 8 个 int32 元素，放入一个寄存器中的 8 个 int16 元素中去，饱和处理", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.5)
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
        ))
        self.wait(0.75)

        self.play(*[FadeOut(v1_rects[i]) for i in range(n) if i >= n // 2],
                  *[FadeOut(v2_rects[i]) for i in range(n) if i >= n // 2],
                  run_time=1.25)
        self.wait(0.25)
        self.play(*[v1_rects[i].animate.move_to(res_rects[i * 2]) for i in range(n) if i < n // 2],
                  *[v2_rects[i].animate.move_to(res_rects[i * 2 + 1]) for i in range(n) if i < n // 2],
                  run_time=2)
        self.wait(1.25)


class UnpackloEPI16(Scene):
    def construct(self):
        title = Text("_mm_unpacklo_epi16")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.7
        n = globals()['n'] * 2
        opacities = opacities2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.25)
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.25).shift(DOWN)

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
        ))
        self.wait(0.75)

        self.play(*[FadeOut(v1_rects[i]) for i in range(n) if i >= n // 2],
                  *[FadeOut(v2_rects[i]) for i in range(n) if i >= n // 2],
                  run_time=1.25)
        self.wait(0.25)
        self.play(*[v1_rects[i].animate.move_to(res_rects[i * 2]) for i in range(n) if i < n // 2],
                  *[v2_rects[i].animate.move_to(res_rects[i * 2 + 1]) for i in range(n) if i < n // 2],
                  run_time=2)
        self.wait(1.25)


class UnpackhiEPI16(Scene):
    def construct(self):
        title = Text("_mm_unpackhi_epi16")
        self.play(Write(title, run_time=1.25))
        self.wait(1)
        self.play(title.animate.shift(UP * 3.5), run_time=0.25)

        hwps = dict(**globals()['hwps'])
        hwps['width'] /= 1.7
        n = globals()['n'] * 2
        opacities = opacities2

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.25).shift(UP)
        res_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=RED) for i in range(n)]).arrange(endian, buff=0.25)
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.25).shift(DOWN)

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
        ))
        self.wait(0.75)

        self.play(*[FadeOut(v1_rects[i]) for i in range(n) if i < n // 2],
                  *[FadeOut(v2_rects[i]) for i in range(n) if i < n // 2],
                  run_time=1.25)
        self.wait(0.25)
        self.play(*[v1_rects[i + n // 2].animate.move_to(res_rects[i * 2]) for i in range(n) if i < n // 2],
                  *[v2_rects[i + n // 2].animate.move_to(res_rects[i * 2 + 1]) for i in range(n) if i < n // 2],
                  run_time=2)
        self.wait(1.25)


class BroadcastdEPI32(Scene):
    def construct(self):
        title = Text("_mm_broadcastd_epi32")
        title2 = Text("全部 int32 元素设为和最低 32 位相同的值", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5)
        v1_texts = Group(*[MathTex('x_' + str(i)).move_to(v1_rects[i]) for i in range(n)])

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v1_texts),
        ))
        self.wait(0.85)

        for i in range(1, n):
            self.play(FadeOut(v1_texts[i]), v1_texts[0].copy().animate.move_to(v1_rects[i]), run_time=1)
        self.wait(1)


class MoveEPI64(Scene):
    def construct(self):
        title = Text("_mm_move_epi64")
        title2 = Text("低 64 位取新的值，高 64 位保留旧的值", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        hwps = dict(**globals()['hwps'])
        hwps['width'] *= 2
        n = globals()['n'] // 2
        opacities = globals()['opacities'][::2]

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5).shift(UP)
        v2_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=GREEN) for i in range(n)]).arrange(endian, buff=0.5).shift(DOWN)

        self.play(AnimationGroup(
            FadeIn(v1_rects),
            FadeIn(v2_rects),
        ))
        self.wait(1)

        self.play(AnimationGroup(
            FadeOut(v1_rects[0], run_time=2),
            *[v1_rects[i].animate(run_time=2).shift(DOWN) for i in range(1, len(v1_rects))],
            v2_rects[0].animate(run_time=2).shift(UP),
            *[FadeOut(v2_rects[i], run_time=2) for i in range(1, len(v2_rects))],
        ))
        self.wait(1.25)


class MovehdupPS(Scene):
    def construct(self):
        title = Text("_mm_movehdup_ps")
        title2 = Text("复制偶数位的 float32，覆盖奇数位", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5)

        self.play(AnimationGroup(
            FadeIn(v1_rects),
        ))
        self.wait(1)

        self.play(AnimationGroup(
            *[v1_rects[i].copy().animate(run_time=2).move_to(v1_rects[i - 1]) for i in range(n) if i % 2 == 1],
            *[FadeOut(v1_rects[i], run_time=2) for i in range(n) if i % 2 == 0],
        ))
        self.wait(1.25)


class MoveldupPS(Scene):
    def construct(self):
        title = Text("_mm_moveldup_ps")
        title2 = Text("复制奇数位的 float32，覆盖偶数位", font_size=32).shift(DOWN * 0.65)
        self.play(AnimationGroup(Write(title), Write(title2), run_time=1.25))
        self.wait(0.85)
        self.play(AnimationGroup(FadeOut(title2), title.animate.shift(UP * 3.5)), run_time=0.25)

        v1_rects = Group(*[Rectangle(**hwps, fill_opacity=opacities[i], fill_color=BLUE) for i in range(n)]).arrange(endian, buff=0.5)

        self.play(AnimationGroup(
            FadeIn(v1_rects),
        ))
        self.wait(1)

        self.play(AnimationGroup(
            *[v1_rects[i].copy().animate(run_time=2).move_to(v1_rects[i + 1]) for i in reversed(range(n)) if i % 2 == 0],
            *[FadeOut(v1_rects[i], run_time=2) for i in reversed(range(n)) if i % 2 == 1],
        ))
        self.wait(1.25)
