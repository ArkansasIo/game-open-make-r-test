using RPGCreator.SDK.Attributes;
using RPGCreator.SDK.Modules.Features.Entity;
using RPGCreator.SDK.Types;

namespace TestModule.TestCustomFeature;

[EntityFeature]
public class MyVeryOwnTestDependencyFeature : BaseEntityFeature
{
    public override string FeatureName => "My Very Own Test Dependency Feature";
    public override string FeatureDescription => "Dependency feature used by TestFeature so the sample feature can be enabled.";
    public override URN FeatureUrn => FeatureUrnModule.ToUrnModule("rpgc").ToUrn("MyVeryOwnTestDependencyFeature");
    public override string FeatureIcon => "mdi-link";
}
