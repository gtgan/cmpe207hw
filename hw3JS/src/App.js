import React, { Component } from 'react';
import { Container, Row } from 'reactstrap'

import Header from './Components/Header';
import LiveUsers from './Components/LiveUsers';

class App extends Component {
  render () {
    return  (
      <React.Fragment>
        <Header/>

        <Container>
          <Row>
            <LiveUsers/>
          </Row>
        </Container>
      </React.Fragment>
    )
  }
}


export default App;
