/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Copyright (C) 2026 MuseScore Limited and others
 */

#include <gmock/gmock.h>

#include "context/tests/mocks/globalcontextmock.h"
#include "notation/tests/mocks/notationmock.h"
#include "notation/tests/mocks/notationinteractionmock.h"
#include "playback/tests/mocks/playbackcontrollermock.h"
#include "notation/internal/notationelements.h"
#include "notation/internal/notationnoteinput.h"
#include "engraving/tests/utils/scorerw.h"
#include "engraving/dom/chord.h"
#include "engraving/dom/clef.h"
#include "engraving/dom/factory.h"
#include "engraving/dom/input.h"
#include "engraving/dom/measure.h"
#include "engraving/dom/note.h"
#include "engraving/dom/rest.h"
#include "engraving/dom/segment.h"
#include "../internal/notationbraille.h"

using namespace mu;
using namespace mu::engraving;
using namespace mu::notation;
using namespace mu::braille;
using namespace testing;

class NotationBrailleNavigationTests : public TestWithParam<std::tuple<BrailleInputMethod, bool>>, public IGetScore
{
public:
	Score* score() const override { return m_score.get(); }
	muse::async::Notification scoreInited() const override { return {}; }

	void SetUp() override
	{
		m_score.reset(ScoreRW::readScore(muse::String(u"data/testPitches.mscx")));
		ASSERT_TRUE(m_score);
		m_score->doLayout();
		m_first = toChord(m_score->firstMeasure()->firstChordRest(0))->upNote();
		m_second = toChord(m_first->chord()->segment()->nextCR(0)->element(0))->upNote();

		m_context = std::make_shared<muse::modularity::Context>(42);
		m_global = std::make_shared<NiceMock<context::GlobalContextMock>>();
		m_notation = std::make_shared<NiceMock<NotationMock>>();
		m_interaction = std::make_shared<NiceMock<NotationInteractionMock>>();
		m_playback = std::make_shared<NiceMock<playback::PlaybackControllerMock>>();
		m_input = std::make_shared<NotationNoteInput>(this, m_interaction.get(), nullptr, m_context);
		m_elements = std::make_shared<NotationElements>(this);
		ON_CALL(*m_global, currentNotation()).WillByDefault(Return(m_notation));
		ON_CALL(*m_notation, interaction()).WillByDefault(Return(m_interaction));
		ON_CALL(*m_notation, elements()).WillByDefault(Return(m_elements));
		ON_CALL(*m_interaction, noteInput()).WillByDefault(Return(m_input));
		muse::modularity::ioc(m_context)->registerExport<context::IGlobalContext>("braille_tests", m_global);
		muse::modularity::ioc(m_context)->registerExport<playback::IPlaybackController>("braille_tests", m_playback);

		m_score->inputState().setNoteEntryMode(std::get<1>(GetParam()));
		m_score->inputState().setTrack(m_second->track());
		m_score->inputState().moveInputPos(m_second->chord());
		m_braille = std::make_unique<NotationBraille>(m_context);
		m_braille->setInputMethod(std::get<0>(GetParam()));
		// Several Braille cells map to each note; the separator maps to no item.
		m_braille->brailleEngravingItemList()->insert(MAX_LIVE_BRAILLE_LENGTH,
			BrailleEngravingItem(BEIType::EngravingItem, m_first->chord(), QStringLiteral("abc")));
		m_braille->brailleEngravingItemList()->insert(MAX_LIVE_BRAILLE_LENGTH,
			BrailleEngravingItem(BEIType::EngravingItem, m_second, QStringLiteral("def")));
		m_braille->brailleEngravingItemList()->insert(MAX_LIVE_BRAILLE_LENGTH,
			BrailleEngravingItem(BEIType::EndOfLine, nullptr, QStringLiteral(" ")));
	}

	void TearDown() override
	{
		m_braille.reset();
		m_input.reset();
		muse::modularity::removeIoC(m_context);
	}

	std::unique_ptr<MasterScore> m_score;
	muse::modularity::ContextPtr m_context;
	std::shared_ptr<NiceMock<context::GlobalContextMock>> m_global;
	std::shared_ptr<NiceMock<NotationMock>> m_notation;
	std::shared_ptr<NiceMock<NotationInteractionMock>> m_interaction;
	std::shared_ptr<NiceMock<playback::PlaybackControllerMock>> m_playback;
	std::shared_ptr<NotationNoteInput> m_input;
	std::shared_ptr<NotationElements> m_elements;
	std::unique_ptr<NotationBraille> m_braille;
	Note* m_first = nullptr;
	Note* m_second = nullptr;
};

TEST_P(NotationBrailleNavigationTests, SelectsAndAuditionsOncePerItem)
{
	EXPECT_CALL(*m_playback, playElements(ElementsAre(m_first->chord()), _, _)).Times(1);
	EXPECT_CALL(*m_interaction, select(ElementsAre(m_first), _, _)).Times(1);
	m_braille->setCursorPosition(0);
	m_braille->setCursorPosition(1);
	m_braille->setCursorPosition(2);
	EXPECT_EQ(m_braille->currentEngravingItem(), m_first);
	EXPECT_EQ(m_braille->cursorPosition().val, 2);
	EXPECT_EQ(m_input->state().segment(), std::get<1>(GetParam())
		? m_first->chord()->segment() : m_second->chord()->segment());
	EXPECT_EQ(m_input->isNoteInputMode(), std::get<1>(GetParam()));

	EXPECT_CALL(*m_playback, playElements(ElementsAre(m_second), _, _)).Times(1);
	EXPECT_CALL(*m_interaction, select(ElementsAre(m_second), _, _)).Times(1);
	m_braille->setCursorPosition(4);
	EXPECT_EQ(m_braille->currentEngravingItem(), m_second);
	EXPECT_EQ(m_input->state().segment(), m_second->chord()->segment());
}

TEST_P(NotationBrailleNavigationTests, ReturningToNoteAuditionsAgain)
{
	EXPECT_CALL(*m_playback, playElements(ElementsAre(m_first->chord()), _, _)).Times(2);
	EXPECT_CALL(*m_interaction, select(ElementsAre(m_first), _, _)).Times(2);
	EXPECT_CALL(*m_playback, playElements(ElementsAre(m_second), _, _)).Times(1);
	EXPECT_CALL(*m_interaction, select(ElementsAre(m_second), _, _)).Times(1);
	m_braille->setCursorPosition(1);
	m_braille->setCursorPosition(4);
	m_braille->setCursorPosition(1);
}

TEST_P(NotationBrailleNavigationTests, AutomaticItemPositionUpdatesDoNotNavigate)
{
	EXPECT_CALL(*m_playback, playElements(_, _, _)).Times(0);
	EXPECT_CALL(*m_interaction, select(A<const std::vector<EngravingItem*>&>(), _, _)).Times(0);
	m_braille->setCurrentItemPosition(0, 2);
	m_braille->setCurrentItemPosition(3, 5);
	EXPECT_EQ(m_input->state().segment(), m_second->chord()->segment());
}

TEST_P(NotationBrailleNavigationTests, UnmappedAndInvalidPositionsDoNotNavigate)
{
	EXPECT_CALL(*m_playback, playElements(_, _, _)).Times(0);
	EXPECT_CALL(*m_interaction, select(A<const std::vector<EngravingItem*>&>(), _, _)).Times(0);
	m_braille->setCursorPosition(-1);
	m_braille->setCursorPosition(6);
	m_braille->setCursorPosition(7);
	m_braille->setCursorPosition(100);
	EXPECT_EQ(m_input->state().segment(), m_second->chord()->segment());
	EXPECT_EQ(m_braille->cursorPosition().val, 7);
}

TEST_P(NotationBrailleNavigationTests, NoOpenNotationDoesNotNavigate)
{
	ON_CALL(*m_global, currentNotation()).WillByDefault(Return(nullptr));
	EXPECT_CALL(*m_playback, playElements(_, _, _)).Times(0);
	m_braille->setCursorPosition(1);
}

TEST_P(NotationBrailleNavigationTests, RestNavigationMovesInputTrackAndSegment)
{
	Rest* rest = Factory::createRest(m_first->chord()->segment());
	rest->setTrack(1);
	m_first->chord()->segment()->add(rest);
	m_braille->brailleEngravingItemList()->insert(MAX_LIVE_BRAILLE_LENGTH,
		BrailleEngravingItem(BEIType::EngravingItem, rest, QStringLiteral("rst")));
	EXPECT_CALL(*m_interaction, select(ElementsAre(rest), _, _)).Times(1);
	m_braille->setCursorPosition(8);
	EXPECT_EQ(m_input->state().segment(), std::get<1>(GetParam())
		? rest->segment() : m_second->chord()->segment());
	EXPECT_EQ(m_input->state().track(), std::get<1>(GetParam()) ? 1 : m_second->track());
}

TEST_P(NotationBrailleNavigationTests, NonMusicalItemDoesNotMoveInputPosition)
{
	std::unique_ptr<Clef> clef(Factory::createClef(m_first->chord()->segment()));
	m_braille->brailleEngravingItemList()->insert(MAX_LIVE_BRAILLE_LENGTH,
		BrailleEngravingItem(BEIType::EngravingItem, clef.get(), QStringLiteral("clef")));
	EXPECT_CALL(*m_interaction, select(ElementsAre(clef.get()), _, _)).Times(1);
	m_braille->setCursorPosition(8);
	EXPECT_EQ(m_input->state().segment(), m_second->chord()->segment());
	EXPECT_EQ(m_input->state().track(), m_second->track());
}

TEST_P(NotationBrailleNavigationTests, SameCellCanReferToANewItemAfterRefresh)
{
	EXPECT_CALL(*m_playback, playElements(ElementsAre(m_first->chord()), _, _)).Times(1);
	EXPECT_CALL(*m_playback, playElements(ElementsAre(m_second), _, _)).Times(1);
	m_braille->setCursorPosition(1);
	m_braille->brailleEngravingItemList()->clear();
	m_braille->brailleEngravingItemList()->insert(MAX_LIVE_BRAILLE_LENGTH,
		BrailleEngravingItem(BEIType::EngravingItem, m_second, QStringLiteral("def")));
	m_braille->setCursorPosition(1);
	EXPECT_EQ(m_braille->currentEngravingItem(), m_second);
}

INSTANTIATE_TEST_SUITE_P(InputMethods, NotationBrailleNavigationTests,
	Combine(Values(BrailleInputMethod::SixKey, BrailleInputMethod::MuseScoreShortcuts), Bool()));
