/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore Limited and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>

#include <QFile>

#include "engraving/tests/utils/scorerw.h"
#include "engraving/tests/utils/scorecomp.h"

#include "engraving/dom/masterscore.h"
#include "engraving/dom/measure.h"
#include "../internal/braille.h"

using namespace mu::engraving;

static const String BRAILLE_DIR(u"data/");

class Braille_Tests : public ::testing::Test
{
public:
    void brailleSaveTest(const char* file);
};

static bool saveBraille(MasterScore* score, const String& saveName)
{
    QFile file(saveName);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    bool res = Braille(score).write(file);
    file.close();
    return res;
}

static bool saveCompareBrailleScore(MasterScore* score, const String& saveName, const String& compareWithLocalPath)
{
    EXPECT_TRUE(saveBraille(score, saveName));
    return ScoreComp::compareFiles(saveName,  ScoreRW::rootPath() + u"/" + compareWithLocalPath);
}

void Braille_Tests::brailleSaveTest(const char* file)
{
    String fileName = String::fromUtf8(file);
    MasterScore* score = ScoreRW::readScore(BRAILLE_DIR + fileName + u".mscx", false);
    EXPECT_TRUE(score);
    score->doLayout();
    EXPECT_TRUE(saveCompareBrailleScore(score, fileName + ".brf", BRAILLE_DIR + fileName + "_ref.brf"));
    delete score;
}

TEST_F(Braille_Tests, pitches) {
    brailleSaveTest("testPitches");
}
TEST_F(Braille_Tests, octaveMarks) {
    brailleSaveTest("testOctaveMarks");
}
TEST_F(Braille_Tests, tempo1) {
    brailleSaveTest("testTempo_Example_1.8.1.1_MBC2015");
}
TEST_F(Braille_Tests, tempo2) {
    brailleSaveTest("testTempo_Example_1.8.1.2_MBC2015");
}
TEST_F(Braille_Tests, sectionalBarline) {
    brailleSaveTest("testBarline_Example_1.10.3.2_MBC2015");
}
TEST_F(Braille_Tests, specialBarline) {
    brailleSaveTest("testBarline_Example_1.10.1.1_MBC2015");
}
TEST_F(Braille_Tests, notes) {
    brailleSaveTest("testNotes_Example_2.1_MBC2015");
}
TEST_F(Braille_Tests, octavesNoChords) {
    // TODO a,b,c section names not exported
    brailleSaveTest("testOctavesNoChords_Example_3.2.2.1_MBC2015");
}
TEST_F(Braille_Tests, clefs) {
    brailleSaveTest("testClefs_Example_4.2_MBC2015");
}
TEST_F(Braille_Tests, mmrests) {
    brailleSaveTest("testMMRests_Example_5.3.1_MBC2015");
}
TEST_F(Braille_Tests, accidentals) {
    brailleSaveTest("testAccidentals_Example_6.1_MBC2015");
}
TEST_F(Braille_Tests, quarterAccidentals) {
    brailleSaveTest("testQToneAccidentals_Example_6.3_MBC2015");
}
TEST_F(Braille_Tests, keySigs) {
    // a bit changed. the second key sig does not have naturals
    brailleSaveTest("testKeySig_Example_6.5_MBC2015");
}
TEST_F(Braille_Tests, timeSignature) {
    brailleSaveTest("testTimeSig_Example_7.1_MBC2015");
}
TEST_F(Braille_Tests, triplets) {
    brailleSaveTest("testTriplets_Example_8.4_MBC2015");
}
TEST_F(Braille_Tests, tuplets) {
    brailleSaveTest("testTuplets_Example_8.5_MBC2015");
}
TEST_F(Braille_Tests, chords1) {
    brailleSaveTest("testChords_Example_9.1.MBC2015");
}
TEST_F(Braille_Tests, chords2) {
// TODO 9.1.1.d octave mark in unison intervals
    brailleSaveTest("testChords_Example_9.1.1_MBC2015");
}
TEST_F(Braille_Tests, chords3) {
    brailleSaveTest("testChords_Example_9.2.1_MBC2015");
}
TEST_F(Braille_Tests, chords4) {
    brailleSaveTest("testChords_Example_9.2.2_MBC2015");
}
TEST_F(Braille_Tests, ties1) {
    brailleSaveTest("testTie");
}
TEST_F(Braille_Tests, ties2) {
    brailleSaveTest("testTie_Example_10.1_MBC2015");
}
TEST_F(Braille_Tests, tiesChords1) {
    brailleSaveTest("testTie_Example_10.2.1_MBC2015");
}
TEST_F(Braille_Tests, tiesChords2) {
    brailleSaveTest("testTie_Example_10.2.2_MBC2015");
}
TEST_F(Braille_Tests, tiesChords3) {
    brailleSaveTest("testTie_Example_10.2.3_MBC2015");
}
TEST_F(Braille_Tests, voices1) {
    brailleSaveTest("testVoices_Example_11.1.1.1_MBC2015");
}
TEST_F(Braille_Tests, voices2) {
    brailleSaveTest("testVoices_Example_11.1.1.2_MBC2015");
}
TEST_F(Braille_Tests, voices3) {
    brailleSaveTest("testVoices_Example_11.1.1.3_MBC2015");
}
TEST_F(Braille_Tests, voices4) {
    brailleSaveTest("testVoices_Example_11.1.1.4_MBC2015");
}
TEST_F(Braille_Tests, voices5) {
    brailleSaveTest("testVoices_Example_11.1.4.1_MBC2015");
}
TEST_F(Braille_Tests, slursShort) {
    brailleSaveTest("testSlur_Example_13.2_MBC2015");
}
TEST_F(Braille_Tests, slursLong) {
    brailleSaveTest("testSlur_Example_13.3.b_MBC2015");
}
TEST_F(Braille_Tests, slursWithRest) {
    // the Braille ref does not use part measure repeats
    brailleSaveTest("testSlur_Example_13.3.2_MBC2015");
}
TEST_F(Braille_Tests, slursLayered) {
    // the Braille ref uses bracket slurs even if layered instead of doubled-slur
    brailleSaveTest("testSlur_Example_13.3.3_MBC2015");
}
TEST_F(Braille_Tests, slursShortConvergence) {
    brailleSaveTest("testSlur_Example_13.4.1_MBC2015");
}
TEST_F(Braille_Tests, slursMixConvergence) {
    brailleSaveTest("testSlur_Example_13.4.2_b_MBC2015");
}
TEST_F(Braille_Tests, slursMixAndTies) {
    brailleSaveTest("testSlur_Example_13.5.1_b_MBC2015");
}
TEST_F(Braille_Tests, tremolo) {
    brailleSaveTest("testTremolo_Example_14.2.1_MBC2015");
}
TEST_F(Braille_Tests, tremoloAlt) {
    brailleSaveTest("testTremoloAlt_Example_14.3.1_MBC2015");
}
TEST_F(Braille_Tests, fingering1) {
    brailleSaveTest("testFingering_Example_15.1.1_MBC2015");
}
TEST_F(Braille_Tests, fingering2) {
    brailleSaveTest("testFingering_Example_15.2.1_MBC2015");
}
TEST_F(Braille_Tests, graceNotes) {
    // TODO: last measure with doubling grace mark for >4 grace notes
    brailleSaveTest("testGrace_Example_16.2.1_MBC2015");
}
TEST_F(Braille_Tests, graceChords) {
    brailleSaveTest("testGrace_Example_16.2.1.1_MBC2015");
}
TEST_F(Braille_Tests, ornaments) {
    brailleSaveTest("testOrnaments_Example_16.5_MBC2015");
}
TEST_F(Braille_Tests, glissando) {
    brailleSaveTest("testGlissando_Example_16.6.1_MBC2015");
}
TEST_F(Braille_Tests, repeats) {
    brailleSaveTest("testRepeats_Example_17.1.1_MBC2015");
}
TEST_F(Braille_Tests, voltas1) {
    brailleSaveTest("testVolta_Example_17.1.1.1_MBC2015");
}
TEST_F(Braille_Tests, voltas2) {
    brailleSaveTest("testVolta_Example_17.1.1.2_MBC2015");
}
TEST_F(Braille_Tests, voltas3) {
    brailleSaveTest("testVolta_Example_17.1.1.3_MBC2015");
}
TEST_F(Braille_Tests, testMarkersJumps) {
    brailleSaveTest("testJumps_Example_20.2.1_MBC2015");
}
TEST_F(Braille_Tests, breath) {
    brailleSaveTest("testBreaths_Example_22.2.1_MBC2015");
}
TEST_F(Braille_Tests, articulations) {
    brailleSaveTest("testArticulations_Example_22.1_MBC2015");
}
TEST_F(Braille_Tests, hairpins) {
    // removed the 4th measure from the example as MuseScore does not have a representations for mordents with accidentals
    brailleSaveTest("testHairpins_Example_22.3.3.2_MBC2015");
}
TEST_F(Braille_Tests, sectionBreak) {
    brailleSaveTest("testSectionBreak");
}

TEST_F(Braille_Tests, convertMeasureMatchesConvertMeasuresSingle) {
    MasterScore* score = ScoreRW::readScore(BRAILLE_DIR + u"testPitches.mscx", false);
    EXPECT_TRUE(score);
    score->doLayout();

    Measure* m1 = score->firstMeasure();
    EXPECT_TRUE(m1);

    BrailleEngravingItemList viaConvertMeasure;
    Braille(score).convertMeasure(m1, &viaConvertMeasure);

    BrailleEngravingItemList viaConvertMeasures;
    Braille(score).convertMeasures({ m1 }, &viaConvertMeasures);

    EXPECT_EQ(viaConvertMeasure.brailleStr(), viaConvertMeasures.brailleStr());

    delete score;
}

TEST_F(Braille_Tests, convertMeasuresCarriesOctaveContextAcrossGroup) {
    MasterScore* score = ScoreRW::readScore(BRAILLE_DIR + u"testPitches.mscx", false);
    EXPECT_TRUE(score);
    score->doLayout();

    Measure* m1 = score->firstMeasure();
    EXPECT_TRUE(m1);
    Measure* m2 = m1 ? m1->nextMeasure() : nullptr;
    EXPECT_TRUE(m2);

    // Render m1 alone, to know the exact length of its contribution within the group.
    BrailleEngravingItemList m1Solo;
    Braille(score).convertMeasure(m1, &m1Solo);

    // Render m2 alone: a fresh Braille instance has no previous-note context,
    // so the first note of m2 always gets an explicit (unconditional) octave mark.
    BrailleEngravingItemList m2Solo;
    Braille(score).convertMeasure(m2, &m2Solo);

    // Render m1+m2 as one group: octave/clef/key context must carry from m1 into
    // m2, so m2's first note is marked based on the actual interval from m1's last
    // note instead of unconditionally - i.e. m2's rendering differs from m2Solo.
    BrailleEngravingItemList group;
    Braille(score).convertMeasures({ m1, m2 }, &group);

    QString groupStr = group.brailleStr();
    QString m1InGroup = groupStr.left(m1Solo.brailleStr().length());
    QString m2InGroup = groupStr.mid(m1Solo.brailleStr().length() + 1); // +1 skips the newline separator

    EXPECT_EQ(m1InGroup, m1Solo.brailleStr());
    EXPECT_NE(m2InGroup, m2Solo.brailleStr());

    delete score;
}

TEST_F(Braille_Tests, convertMeasuresSectionBySectionRunsBarsHorizontally) {
    MasterScore* score = ScoreRW::readScore(BRAILLE_DIR + u"testPitches.mscx", false);
    EXPECT_TRUE(score);
    score->doLayout();

    Measure* m1 = score->firstMeasure();
    EXPECT_TRUE(m1);
    Measure* m2 = m1 ? m1->nextMeasure() : nullptr;
    EXPECT_TRUE(m2);

    // Bar-over-bar (today's behavior): each measure on its own line.
    BrailleEngravingItemList barOverBar;
    Braille(score).convertMeasures({ m1, m2 }, &barOverBar);
    EXPECT_TRUE(barOverBar.brailleStr().contains(QChar('\n')));

    // Section-by-section: consecutive bars of the same (single) staff run
    // together on one continuous line instead of one bar per newline. Both
    // modes carry the same octave/clef/key context across the group (see
    // convertMeasuresCarriesOctaveContextAcrossGroup), so with a single
    // staff and no lyrics, the only difference from bar-over-bar here is
    // the newline bar-over-bar inserts between the two measures.
    BrailleEngravingItemList sectionBySection;
    Braille(score).convertMeasuresSectionBySection({ m1, m2 }, &sectionBySection);

    QString expected = barOverBar.brailleStr();
    expected.remove(QChar('\n'));

    EXPECT_FALSE(sectionBySection.brailleStr().contains(QChar('\n')));
    EXPECT_EQ(sectionBySection.brailleStr(), expected);

    delete score;
}
