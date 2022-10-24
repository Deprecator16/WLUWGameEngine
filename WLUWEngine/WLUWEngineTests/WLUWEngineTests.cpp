#include "pch.h"
#include "CppUnitTest.h"
#include "Vector2.h"
#include "WComponentBase.h"
#include "WObject.h"
#include "TypeIdManager.h"
#include "specializations.h"

#include <math.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace WLUWEngineTests
{
	TEST_CLASS(Vector2_Tests)
	{
	public:
		TEST_METHOD(DefaultConstructor_T)
		{
			WLUW::Vector2 v;
			Assert::AreEqual(0.0, v.x);
			Assert::AreEqual(0.0, v.y);
		}
		TEST_METHOD(FloatConstructor_T)
		{
			double x = -0.73f;
			double y = 9.383f;
			WLUW::Vector2 v(x, y);
			Assert::AreEqual(x, v.x);
			Assert::AreEqual(y, v.y);
		}

		TEST_METHOD(size_T)
		{
			WLUW::Vector2 v(-0.73, 9.383);
			double size = v.size();
			double squareroot = sqrt(pow(v.x, 2.0) + pow(v.y, 2.0));

			Assert::AreEqual(size, squareroot);
		}

		TEST_METHOD(AdditionWithVector2Operator_T)
		{
			WLUW::Vector2 v(-0.73f, 9.383f);
			WLUW::Vector2 w(0.73f, -9.38f);
			WLUW::Vector2 comparator(v.x + w.x, v.y + w.y);

			Assert::AreEqual(v + w, comparator);
		}

		TEST_METHOD(AdditionWithFloatOperator_T)
		{
			WLUW::Vector2 v(-0.73f, 9.383f);
			double a = 3.7f;
			WLUW::Vector2 comparator(v.x + a, v.y + a);

			Assert::AreEqual(v + a, comparator);
		}

		TEST_METHOD(SubtractionWithVector2Operator_T)
		{
			WLUW::Vector2 v(-0.73f, 9.383f);
			WLUW::Vector2 w(0.73f, -9.38f);
			WLUW::Vector2 comparator(v.x - w.x, v.y - w.y);

			Assert::AreEqual(v - w, comparator);
		}

		TEST_METHOD(SubtractionWithFloatOperator_T)
		{
			WLUW::Vector2 v(-0.73f, 9.383f);
			double a = 3.7f;
			WLUW::Vector2 comparator(v.x - a, v.y - a);

			Assert::AreEqual(v - a, comparator);
		}

		TEST_METHOD(MultiplicationWithVector2Operator_T)
		{
			WLUW::Vector2 v(-0.73f, 9.383f);
			WLUW::Vector2 w(0.73f, -9.38f);
			WLUW::Vector2 comparator(v.x * w.x, v.y * w.y);

			Assert::AreEqual(v * w, comparator);
		}

		TEST_METHOD(MultiplicationWithFloatOperator_T)
		{
			WLUW::Vector2 v(-0.73f, 9.383f);
			double a = 3.7f;
			WLUW::Vector2 comparator(v.x * a, v.y * a);

			Assert::AreEqual(v * a, comparator);
		}

		TEST_METHOD(DivisionWithVector2Operator_T)
		{
			WLUW::Vector2 v(-0.73f, 9.383f);
			WLUW::Vector2 w(0.73f, -9.38f);
			WLUW::Vector2 comparator(v.x / w.x, v.y / w.y);

			Assert::AreEqual(v / w, comparator);
		}

		TEST_METHOD(DivisionWithFloatOperator_T)
		{
			WLUW::Vector2 v(-0.73f, 9.383f);
			double a = 3.7f;
			WLUW::Vector2 comparator(v.x / a, v.y / a);

			Assert::AreEqual(v / a, comparator);
		}

		TEST_METHOD(NegateOperator_T)
		{
			WLUW::Vector2 v(-0.73f, 9.38f);
			WLUW::Vector2 w(0.73f, -9.38f);

			Assert::AreEqual(-v, w);
		}

		TEST_METHOD(EquivalenceOperator_T)
		{
			WLUW::Vector2 v(0.73f, 9.38f);
			WLUW::Vector2 w(0.73f, 9.38f);

			Assert::IsTrue(v == w);
		}

		TEST_METHOD(IndexOperator_T)
		{
			WLUW::Vector2 v(0.73f, 9.38f);

			Assert::AreEqual(v.x, v[0]);
			Assert::AreEqual(v.y, v[1]);
		}
	};

	TEST_CLASS(WComponents_Tests)
	{
		TEST_METHOD(UniqueClassID_T)
		{
			Assert::AreEqual(WLUW::TypeIdManager<WLUW::WComponentBase>::getClassUniqueID<WLUW::WObject>(), 1);
			Assert::AreEqual(WLUW::TypeIdManager<WLUW::WComponentBase>::getClassUniqueID<WLUW::WObject>(), 1);
			Assert::AreEqual(WLUW::TypeIdManager<WLUW::WComponentBase>::getClassUniqueID<WLUW::Vector2>(), 2);
			Assert::AreEqual(WLUW::TypeIdManager<WLUW::WComponentBase>::getClassUniqueID<WLUW::Vector2>(), 2);
			Assert::AreEqual(WLUW::TypeIdManager<WLUW::WComponentBase>::getClassUniqueID<WLUW::WObject>(), 1);
		}

		TEST_METHOD(UniqueInstanceID_T)
		{
			// Causes an 'LNK2001 unresolved external symbol' error
			/*Assert::AreEqual(WLUW::TypeIdManager<WLUW::WComponentBase>::getNewID(), 1);
			Assert::AreEqual(WLUW::TypeIdManager<WLUW::WComponentBase>::getNewID(), 2);
			Assert::AreEqual(WLUW::TypeIdManager<WLUW::WComponentBase>::getNewID(), 3);
			Assert::AreEqual(WLUW::TypeIdManager<WLUW::WComponentBase>::getNewID(), 4);*/
		}
	};
}
