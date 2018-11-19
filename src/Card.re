let component = ReasonReact.statelessComponent("Card");

let make = (~name, ~description, ~href, _children) => {
  ...component,
  render: _self =>
    <div>
      <h3>
        <a href target="_blank" rel="noopener noreferrer">
          {ReasonReact.string(name)}
        </a>
      </h3>
      <p> {ReasonReact.string(description)} </p>
    </div>,
};